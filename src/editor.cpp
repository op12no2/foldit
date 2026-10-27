//{{{}}}
// editor.cpp : the terminal editor

//{{{  includes
#define NCURSES_WIDECHAR 1
#include <ncurses.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <sstream>
#include "editor.h"
//}}}
//{{{  keys
// function keys are offset so they never clash with unicode characters
static const int KB = 0x1000000;
#define K(x)    (KB + (x))
#define CTRL(c) ((c) & 0x1f)
// bracketed paste start and end, as defined with define_key()
static const int PASTE_START = KEY_MAX + 1, PASTE_END = KEY_MAX + 2;
//}}}
//{{{  path helpers
static std::string cwd()
{
    char buf[PATH_MAX];
    return getcwd(buf, sizeof(buf)) ? buf : ".";
}

static std::string abs_path(const std::string &p)
{
    char buf[PATH_MAX];
    if (realpath(p.c_str(), buf)) return buf;
    if (!p.empty() && p[0] == '/') return p;
    return cwd() + "/" + p;
}

static std::string dir_of(const std::string &p)
{
    size_t s = p.rfind('/');
    return s == std::string::npos ? "." : (s == 0 ? "/" : p.substr(0, s));
}

static std::string ext_of(const std::string &p)
{
    std::string b = p.substr(p.rfind('/') + 1);
    size_t d = b.rfind('.');
    return d == std::string::npos || d == 0 ? "" : b.substr(d + 1);
}

static std::string short_name(const std::string &ap)
{
    std::string c = cwd() + "/";
    if (ap.compare(0, c.size(), c) == 0) return ap.substr(c.size());
    const char *h = getenv("HOME");
    if (h && ap.compare(0, strlen(h), h) == 0) return "~" + ap.substr(strlen(h));
    return ap;
}

// turn a file reference into a path: ~ and Windows drive paths are understood,
// anything else relative is taken from the folder of the referencing file
static std::string ref_path(std::string r, const std::string &dir)
{
    for (auto &c : r) if (c == '\\') c = '/';
    if (r.size() >= 2 && r[0] == '~' && r[1] == '/')
    {
        const char *h = getenv("HOME");
        return std::string(h ? h : "") + r.substr(1);
    }
    if (r.size() >= 3 && isalpha((unsigned char)r[0]) && r[1] == ':' && r[2] == '/')
        return std::string("/mnt/") + (char)tolower((unsigned char)r[0]) + r.substr(2);
    if (!r.empty() && r[0] == '/') return r;
    return dir + "/" + r;
}

static bool exists(const std::string &p)
{
    struct stat st;
    return stat(p.c_str(), &st) == 0;
}
//}}}
//{{{  tree helpers
static std::vector<int> path_of(const Line *f)
{
    std::vector<int> p;
    for (; f->parent; f = f->parent) p.insert(p.begin(), f->parent->index_of(f));
    return p;
}

static Line *resolve(Line *root, const std::vector<int> &path)
{
    Line *f = root;
    for (int i : path)
    {
        if (i < 0 || i >= (int)f->kids.size() || !f->kids[i]->fold) break;
        f = f->kids[i].get();
    }
    return f;
}

static std::string preview(const ustr &s)
{
    std::string e = encode(trim(s));
    return e.size() > 50 ? e.substr(0, 50) + "..." : e;
}
//}}}

//{{{  documents
//{{{  load_doc
Doc *Editor::load_doc(const std::string &path, std::string &err)
{
    std::string ap = abs_path(path);
    for (auto &d : docs)
        if (d->path == ap) return d.get();

    std::unique_ptr<Doc> d(new Doc);
    d->path = ap;
    d->name = short_name(ap);

    std::string raw;
    struct stat st;
    if (stat(ap.c_str(), &st) == 0)
    {
        if (S_ISDIR(st.st_mode)) { err = path + " is a directory"; return nullptr; }
        std::ifstream f(ap, std::ios::binary);
        if (!f) { err = "can't read " + path; return nullptr; }
        std::stringstream ss;
        ss << f.rdbuf();
        raw = ss.str();
        d->exists = true;
        d->mtime  = st.st_mtime;
    }

    size_t nl = raw.find('\n');
    d->crlf = nl != std::string::npos && nl > 0 && raw[nl - 1] == '\r';

    ustr text = decode(raw);
    d->lang = cfg.detect(text.substr(0, 4096), ext_of(ap));
    ParseResult r = parse_folds(text, *d->lang);
    d->root = std::move(r.root);

    msg = d->name + ": " + d->lang->name + (d->exists ? "" : " (new file)");
    if (r.under_indented) msg += "; " + std::to_string(r.under_indented) + " lines less indented than their fold";
    if (r.extra_ends)     msg += "; " + std::to_string(r.extra_ends) + " stray end marks kept as text";
    if (r.unclosed)       msg += "; " + std::to_string(r.unclosed) + " unclosed folds";

    docs.push_back(std::move(d));
    return docs.back().get();
}
//}}}
//{{{  save_doc
bool Editor::save_doc(Doc *d)
{
    struct stat st;
    bool there = stat(d->path.c_str(), &st) == 0;
    if (d->exists && there && st.st_mtime != d->mtime)
        if (ask(d->name + " has changed on disk since it was loaded. Overwrite? (y/n)", "yn") != 'y')
            return false;

    std::string out = encode(write_folds(*d->root, *d->lang));
    if (d->crlf)
    {
        std::string c;
        c.reserve(out.size() + out.size() / 20);
        for (char ch : out)
        {
            if (ch == '\n') c += '\r';
            c += ch;
        }
        out.swap(c);
    }

    //write a temporary file and rename it over the original, unless it is
    //a symlink, which we write through
    struct stat lst;
    bool link = lstat(d->path.c_str(), &lst) == 0 && S_ISLNK(lst.st_mode);
    std::string target = link ? d->path : d->path + ".foldit~";
    {
        std::ofstream f(target, std::ios::binary | std::ios::trunc);
        f << out;
        f.close();
        if (!f) { msg = "can't write " + target + ": " + strerror(errno); unlink(target.c_str()); return false; }
    }
    if (!link)
    {
        if (there) chmod(target.c_str(), st.st_mode & 07777);
        if (rename(target.c_str(), d->path.c_str()) != 0)
        {
            msg = "can't save " + d->name + ": " + strerror(errno);
            unlink(target.c_str());
            return false;
        }
    }

    if (stat(d->path.c_str(), &st) == 0) d->mtime = st.st_mtime;
    d->exists   = true;
    d->modified = false;
    msg = "saved " + d->name;
    return true;
}
//}}}
//{{{  release_doc
// the view has left a document; ask about saving and forget it.  false if
// the user cancelled.
bool Editor::release_doc(Doc *d)
{
    if (d->modified)
    {
        int a = ask("Save changes to " + d->name + "? (y/n)", "yn");
        if (!a) return false;
        if (a == 'y' && !save_doc(d)) return false;
    }
    if (find_doc == d) find_doc = nullptr;
    for (size_t i = 0; i < docs.size(); i++)
        if (docs[i].get() == d) { docs.erase(docs.begin() + i); break; }
    return true;
}

bool Editor::doc_in_stack(Doc *d) const
{
    for (auto &p : v.stack)
        if (p.doc == d) return true;
    return false;
}
//}}}
//}}}
//{{{  view
void Editor::clamp()
{
    v.fold->ensure_not_empty();
    int n = (int)v.fold->kids.size();
    if (v.cy >= n) v.cy = n - 1;
    if (v.cy < 0)  v.cy = 0;
    int len = disp_len(cur());
    if (v.cx > len) v.cx = len;
    if (v.cx < 0)   v.cx = 0;
    if (v.sel && v.sy >= n) v.sel = false;
}

// a fold is in use if the view, or anywhere we will return to, is inside it
bool Editor::in_use(const Line *f) const
{
    if (f->contains(v.fold)) return true;
    for (auto &p : v.stack)
        if (f->contains(p.fold)) return true;
    return false;
}

ustr Editor::display(const Line *l) const
{
    if (!l->fold) return l->text;
    return ustr(l->indent, U' ') + cfg.fold_prefix + U"  " + l->text;
}
//}}}
//{{{  undo
// snapshot the whole document before each change; a run of typing on one
// line is one change
void Editor::begin_edit(bool typing)
{
    Doc *d = v.doc;
    d->modified = true;
    if (typing && prev_typing == cur())
    {
        typing_in = cur();
        return;
    }
    typing_in = typing ? cur() : nullptr;
    d->undo.push_back({d->root->clone(), path_of(v.fold), v.cy, v.cx, v.top});
    if (d->undo.size() > 200) d->undo.erase(d->undo.begin());
}

void Editor::undo()
{
    Doc *d = v.doc;
    if (d->undo.empty()) { msg = "nothing to undo"; return; }

    //places we will return to in this document are kept as paths across the swap
    std::vector<std::vector<int>> paths;
    for (auto &p : v.stack)
        paths.push_back(p.doc == d ? path_of(p.fold) : std::vector<int>());

    Doc::Snapshot s = std::move(d->undo.back());
    d->undo.pop_back();
    d->root = std::move(s.root);

    for (size_t i = 0; i < v.stack.size(); i++)
        if (v.stack[i].doc == d) v.stack[i].fold = resolve(d->root.get(), paths[i]);
    v.fold = resolve(d->root.get(), s.path);
    v.cy = s.cy; v.cx = s.cx; v.top = s.top;
    v.sel = false;
    d->modified = true;
    msg = "undone";
}
//}}}
//{{{  selection and paste buffers
//{{{  get_sel
// a selection within one line is inline (characters); anything else is a
// run of whole lines, including the lines at both ends
Editor::Sel Editor::get_sel()
{
    Sel s;
    if (!v.sel) return s;
    int n = (int)v.fold->kids.size();
    s.y1 = v.sy; s.x1 = v.sx; s.y2 = v.cy; s.x2 = v.cx;
    if (s.y1 > s.y2 || (s.y1 == s.y2 && s.x1 > s.x2)) { std::swap(s.y1, s.y2); std::swap(s.x1, s.x2); }
    if (s.y1 < 0 || s.y2 >= n) return s;

    if (s.y1 == s.y2)
    {
        if (s.x1 == s.x2) return s;
        Line *l = v.fold->kids[s.y1].get();
        s.any = true;
        if (!l->fold) { s.inl = true; return s; }
        int ts = title_start(l);
        s.x1 = std::max(s.x1, ts);
        s.x2 = std::max(s.x2, ts);
        s.inl = s.x1 < s.x2;        //inside the title, else the whole fold line
        return s;
    }
    s.any = true;
    return s;
}
//}}}
//{{{  delete_sel
bool Editor::delete_sel()
{
    Sel s = get_sel();
    v.sel = false;
    if (!s.any) return true;

    if (s.inl)
    {
        Line *l = v.fold->kids[s.y1].get();
        int off = l->fold ? title_start(l) : 0;
        begin_edit();
        int a = std::min(s.x1 - off, (int)l->text.size());
        int b = std::min(s.x2 - off, (int)l->text.size());
        l->text.erase(a, b - a);
        v.cy = s.y1; v.cx = s.x1;
        return true;
    }

    for (int i = s.y1; i <= s.y2; i++)
        if (v.fold->kids[i]->fold && in_use(v.fold->kids[i].get()))
        { msg = "can't delete a fold that is open"; return false; }
    begin_edit();
    v.fold->kids.erase(v.fold->kids.begin() + s.y1, v.fold->kids.begin() + s.y2 + 1);
    v.cy = s.y1; v.cx = 0;
    clamp();
    return true;
}
//}}}
//{{{  copy_sel
void Editor::copy_sel(bool cut)
{
    Sel s = get_sel();
    if (!s.any) { msg = "nothing selected"; return; }

    std::unique_ptr<Paste> p(new Paste);
    if (s.inl)
    {
        Line *l = v.fold->kids[s.y1].get();
        int off = l->fold ? title_start(l) : 0;
        p->lines.push_back(Line::make_text(l->text.substr(s.x1 - off, s.x2 - s.x1)));
        p->inline_text = true;
    }
    else
        for (int i = s.y1; i <= s.y2; i++) p->lines.push_back(v.fold->kids[i]->clone());

    if (cut && !delete_sel()) return;
    v.sel = false;
    push_paste(std::move(p));
}
//}}}
//{{{  push_paste
void Editor::push_paste(std::unique_ptr<Paste> p, bool to_clipboard)
{
    char t[16];
    time_t now = time(nullptr);
    strftime(t, sizeof(t), "%H:%M", localtime(&now));
    std::string what = p->lines.size() == 1 ? preview(p->lines[0]->text)
                                            : std::to_string(p->lines.size()) + " lines";
    p->label = std::string(t) + "  " + what;
    msg = (p->inline_text ? "copied " : "copied lines: ") + what;
    pastes.push_back(std::move(p));
    while ((int)pastes.size() > cfg.paste_len) pastes.pop_front();
    paste = pastes.back().get();
    if (to_clipboard) mirror(paste);
}
//}}}
//{{{  do_paste
void Editor::do_paste(bool as_lines)
{
    if (!paste) { msg = "nothing to paste"; return; }
    if (v.sel && !delete_sel()) return;

    if (paste->inline_text && !as_lines)
    {
        insert_text(paste->lines[0]->text);
        return;
    }
    begin_edit();
    for (size_t i = 0; i < paste->lines.size(); i++)
    {
        LinePtr c = paste->lines[i]->clone();
        v.fold->adopt(c, v.cy + (int)i);
    }
    v.cx = 0;
}
//}}}
//}}}
//{{{  editing
//{{{  insert_text
bool Editor::insert_text(const ustr &s, bool typing)
{
    if (s.empty()) return true;
    Line *l = cur();
    if (!l->fold)
    {
        begin_edit(typing);
        l->text.insert(v.cx, s);
        v.cx += (int)s.size();
        return true;
    }
    int ts = title_start(l);
    if (v.cx >= ts)
    {
        begin_edit(typing);
        l->text.insert(v.cx - ts, s);
        v.cx += (int)s.size();
        return true;
    }
    if (v.cx <= l->indent && is_blank(s))
    {
        begin_edit(typing);
        l->indent += (int)s.size();
        v.cx += (int)s.size();
        return true;
    }
    msg = "can't type in a fold mark";
    return false;
}
//}}}
//{{{  key_char key_tab
void Editor::key_char(char32_t c)
{
    if (v.sel && !delete_sel()) return;
    insert_text(ustr(1, c), true);
}

void Editor::key_tab()
{
    if (v.sel && !delete_sel()) return;
    int tab = v.doc->lang->indent > 0 ? v.doc->lang->indent : cfg.indent;
    insert_text(ustr(tab - v.cx % tab, U' '), true);
}
//}}}
//{{{  key_enter
void Editor::key_enter()
{
    if (v.sel && !delete_sel()) return;
    Line *l = cur();
    if (l->fold)
    {
        if (v.cx <= l->indent)
        {
            begin_edit();
            LinePtr n = Line::make_text();
            v.fold->adopt(n, v.cy);
            v.cy++;
        }
        else if (v.cx >= disp_len(l))
        {
            begin_edit();
            LinePtr n = Line::make_text();
            v.fold->adopt(n, v.cy + 1);
            v.cy++;
            v.cx = 0;
        }
        else
            msg = "can't split a fold line";
        return;
    }
    begin_edit();
    LinePtr n = Line::make_text(l->text.substr(v.cx));
    l->text.resize(v.cx);
    v.fold->adopt(n, v.cy + 1);
    v.cy++;
    v.cx = 0;
}
//}}}
//{{{  key_delete
void Editor::key_delete()
{
    if (v.sel) { delete_sel(); return; }
    Line *l = cur();
    if (l->fold)
    {
        int ts = title_start(l);
        if (v.cx < l->indent) { begin_edit(true); l->indent--; }
        else if (v.cx >= ts && v.cx < disp_len(l)) { begin_edit(true); l->text.erase(v.cx - ts, 1); }
        return;
    }
    if (v.cx < (int)l->text.size()) { begin_edit(true); l->text.erase(v.cx, 1); return; }
    if (v.cy + 1 >= (int)v.fold->kids.size()) return;
    if (l->text.empty())
    {
        begin_edit();
        v.fold->remove(v.cy);
        v.cx = 0;
        return;
    }
    Line *next = v.fold->kids[v.cy + 1].get();
    if (next->fold) { msg = "can't join a fold to a line"; return; }
    begin_edit();
    l->text += next->text;
    v.fold->remove(v.cy + 1);
}
//}}}
//{{{  key_backspace
void Editor::key_backspace()
{
    if (v.sel) { delete_sel(); return; }
    Line *l = cur();
    if (l->fold)
    {
        int ts = title_start(l);
        if (v.cx > 0 && v.cx <= l->indent) { begin_edit(true); l->indent--; v.cx--; }
        else if (v.cx > ts) { begin_edit(true); l->text.erase(v.cx - ts - 1, 1); v.cx--; }
        return;
    }
    if (v.cx > 0) { begin_edit(true); l->text.erase(v.cx - 1, 1); v.cx--; return; }
    if (v.cy == 0) return;
    Line *prev = v.fold->kids[v.cy - 1].get();
    if (prev->fold && !l->text.empty()) { msg = "can't join a line to a fold"; return; }
    begin_edit();
    if (prev->fold)
        v.cx = disp_len(prev);
    else
    {
        v.cx = (int)prev->text.size();
        prev->text += l->text;
    }
    v.fold->remove(v.cy);
    v.cy--;
}
//}}}
//{{{  cut_line
void Editor::cut_line(bool cut)
{
    Line *l = cur();
    std::unique_ptr<Paste> p(new Paste);
    if (!cut)
        p->lines.push_back(l->clone());
    else
    {
        if (l->fold && in_use(l)) { msg = "can't cut a fold that is open"; return; }
        begin_edit();
        p->lines.push_back(v.fold->remove(v.cy));
        v.cx = 0;
        clamp();
    }
    push_paste(std::move(p));
}
//}}}
//{{{  create_fold
// with lines selected, fold them up, the fold taking their common indent and
// the first non-blank line as its title; otherwise make an empty fold here
void Editor::create_fold()
{
    Sel s = get_sel();
    v.sel = false;
    if (!s.any)
    {
        begin_edit();
        LinePtr f = Line::make_fold(U"", v.cx);
        f->ensure_not_empty();
        v.fold->adopt(f, v.cy);
        v.cx = title_start(cur());
        return;
    }

    int indent = INT_MAX;
    ustr title;
    for (int i = s.y1; i <= s.y2; i++)
    {
        Line *l = v.fold->kids[i].get();
        if (l->fold && in_use(l)) { msg = "can't fold up a fold that is open"; return; }
        if (!l->fold && is_blank(l->text)) continue;
        indent = std::min(indent, l->fold ? l->indent : leading_spaces(l->text));
        if (title.empty()) title = trim(l->text);
    }
    if (indent == INT_MAX) indent = 0;

    begin_edit();
    LinePtr f = Line::make_fold(title, indent);
    for (int i = s.y1; i <= s.y2; i++)
    {
        LinePtr l = v.fold->remove(s.y1);
        if (l->fold) l->indent -= indent;
        else l->text.erase(0, std::min(indent, leading_spaces(l->text)));
        f->adopt(l, (int)f->kids.size());
    }
    v.fold->adopt(f, s.y1);
    v.cy = s.y1;

    //select the title so typing replaces it
    Line *nf = cur();
    v.sel = true;
    v.sy  = v.cy;
    v.sx  = title_start(nf);
    v.cx  = disp_len(nf);
}
//}}}
//{{{  unfold
void Editor::unfold()
{
    Line *l = cur();
    if (!l->fold) { msg = "not a fold"; return; }
    if (in_use(l)) { msg = "can't unfold a fold that is open"; return; }
    begin_edit();
    LinePtr f = v.fold->remove(v.cy);
    int at = v.cy;
    for (auto &k : f->kids)
    {
        if (k->fold) k->indent += f->indent;
        else if (!k->text.empty()) k->text = ustr(f->indent, U' ') + k->text;
        v.fold->adopt(k, at++);
    }
    v.cx = 0;
}
//}}}
//{{{  indent_lines
// shift the selected lines, or all lines in the fold, by one column
void Editor::indent_lines(int dir)
{
    Sel s = get_sel();
    int a = 0, b = (int)v.fold->kids.size() - 1;
    if (s.any) { a = s.y1; b = s.y2; }

    for (int i = a; dir < 0 && i <= b; i++)
    {
        Line *l = v.fold->kids[i].get();
        bool ok = l->fold ? l->indent > 0 : (is_blank(l->text) || l->text[0] == ' ');
        if (!ok) { msg = "a line is already at the left"; return; }
    }
    begin_edit();
    for (int i = a; i <= b; i++)
    {
        Line *l = v.fold->kids[i].get();
        if (l->fold) l->indent += dir;
        else if (!is_blank(l->text))
        {
            if (dir > 0) l->text.insert(0, 1, U' ');
            else l->text.erase(0, 1);
        }
    }
    if (v.cy >= a && v.cy <= b) v.cx += dir;
    if (s.any) v.sx += dir;
    clamp();
}
//}}}
//{{{  insert_date
void Editor::insert_date()
{
    char d[16];
    time_t now = time(nullptr);
    strftime(d, sizeof(d), "%d/%m/%y", localtime(&now));
    ustr s = decode(d);
    if (!cfg.date_tag.empty()) s += U' ' + cfg.date_tag;
    insert_text(s + U' ');
}
//}}}
//}}}
//{{{  navigation
//{{{  enter_fold
// Home enters a fold, or opens a file reference that exists, and otherwise
// goes to the start of the line.  Ctrl+Left only enters, offering to create
// a referenced file that doesn't exist yet.
void Editor::enter_fold(bool explicit_key)
{
    Line *l = cur();
    if (l->fold)
    {
        v.stack.push_back(v);
        v.fold = l;
        v.cy = v.cx = v.top = v.left = 0;
        v.sel = false;
        return;
    }
    std::string path;
    if (cfg.use_file_refs && file_ref(path) && (explicit_key || exists(path)))
    {
        open_ref(path, explicit_key);
        return;
    }
    if (explicit_key) msg = "not a fold";
    else move_to(v.cy, 0, false);
}
//}}}
//{{{  exit_fold
void Editor::exit_fold()
{
    v.sel = false;
    //{{{  optionally drop trailing blank lines
    if (cfg.del_fold_tails)
    {
        auto &k = v.fold->kids;
        while (k.size() > 1 && !k.back()->fold && is_blank(k.back()->text)
               && !k[k.size() - 2]->fold && is_blank(k[k.size() - 2]->text))
        {
            k.pop_back();
            v.doc->modified = true;
        }
    }
    //}}}

    if (Line *p = v.fold->parent)
    {
        if (!v.stack.empty() && v.stack.back().doc == v.doc && v.stack.back().fold == p)
        {
            static_cast<Place &>(v) = v.stack.back();
            v.stack.pop_back();
        }
        else
        {
            v.cy = p->index_of(v.fold);
            v.cx = 0;
            v.fold = p;
        }
        v.left = 0;
        return;
    }

    //at the top of a file: back to whatever referenced it, or quit
    if (v.stack.empty()) { quit_all(); return; }
    Place back = v.stack.back();
    Doc *d = v.doc;
    v.stack.pop_back();
    if (back.doc != d && !doc_in_stack(d) && !release_doc(d))
    {
        v.stack.push_back(back);
        return;
    }
    static_cast<Place &>(v) = back;
    v.left = 0;
}
//}}}
//{{{  file_ref
// a file reference is the first thing in double or single quotes on a line
bool Editor::file_ref(std::string &path)
{
    const ustr &t = cur()->text;
    for (char32_t q : {U'"', U'\''})
    {
        size_t a = t.find(q);
        if (a == ustr::npos) continue;
        size_t b = t.find(q, a + 1);
        if (b == ustr::npos || b == a + 1) continue;
        path = ref_path(encode(t.substr(a + 1, b - a - 1)), dir_of(v.doc->path));
        return true;
    }
    return false;
}
//}}}
//{{{  open_ref
void Editor::open_ref(const std::string &path, bool create)
{
    if (!exists(path))
    {
        if (!create) { msg = short_name(path) + " does not exist"; return; }
        if (ask("Create " + short_name(path) + "? (y/n)", "yn") != 'y') return;
    }
    std::string err;
    Doc *d = load_doc(path, err);
    if (!d) { msg = err; return; }
    v.stack.push_back(v);
    v.doc  = d;
    v.fold = d->root.get();
    v.cy = v.cx = v.top = v.left = 0;
    v.sel = false;
}
//}}}
//{{{  find
// search (ignoring case) forward from the cursor through this fold and the
// folds inside it, then on through the fold find-first was done in
static bool search_in(Line *f, int line, int col, const ustr &pat,
                      const std::function<ustr(const Line *)> &show,
                      const std::function<int(const Line *)> &start,
                      Line *&hf, int &hy, int &hx)
{
    for (int i = line; i < (int)f->kids.size(); i++, col = 0)
    {
        Line *l = f->kids[i].get();
        ustr t = lower(show(l));
        size_t from = std::max(col, start(l));
        size_t at = from <= t.size() ? t.find(pat, from) : ustr::npos;
        if (at != ustr::npos) { hf = f; hy = i; hx = (int)at; return true; }
        if (l->fold && search_in(l, 0, 0, pat, show, start, hf, hy, hx)) return true;
    }
    return false;
}

void Editor::find(bool first)
{
    Line *root;
    int col;
    if (first)
    {
        ustr s = search;
        if (!prompt("Find: ", s) || s.empty()) return;
        search    = s;
        find_doc  = v.doc;
        find_path = path_of(v.fold);
        root = v.fold;
        col  = v.cx;
    }
    else
    {
        if (search.empty() || find_doc != v.doc) { msg = "use Ctrl+F to find first"; return; }
        root = resolve(v.doc->root.get(), find_path);
        if (!root->contains(v.fold)) { msg = "outside the fold Ctrl+F was used in; use Ctrl+F again"; return; }
        col = v.sel ? v.cx : v.cx + 1;
    }

    ustr pat = lower(search);
    auto show  = [this](const Line *l) { return display(l); };
    auto start = [this](const Line *l) { return l->fold ? title_start(l) : 0; };
    Line *f = v.fold, *hf = nullptr;
    int line = v.cy, hy = 0, hx = 0;
    while (!search_in(f, line, col, pat, show, start, hf, hy, hx))
    {
        if (f == root)
        {
            msg = "'" + encode(search) + "' not found";
            if (root->parent) msg += " in fold '" + preview(root->text) + "'";
            return;
        }
        line = f->parent->index_of(f) + 1;
        col  = 0;
        f    = f->parent;
    }

    goto_fold(hf);
    v.cy  = hy;
    v.sel = true;
    v.sy  = hy;
    v.sx  = hx;
    v.cx  = hx + (int)pat.size();
}
//}}}
//{{{  goto_fold
// move the view to a fold in the same document, as if by exiting and
// entering folds, so exiting afterwards retraces the way
void Editor::goto_fold(Line *target)
{
    while (!v.fold->contains(target))
    {
        Line *p = v.fold->parent;
        if (!v.stack.empty() && v.stack.back().doc == v.doc && v.stack.back().fold == p)
            v.stack.pop_back();
        v.fold = p;
    }
    std::vector<Line *> chain;
    for (Line *f = target; f != v.fold; f = f->parent) chain.insert(chain.begin(), f);
    for (Line *f : chain)
    {
        v.cy = v.fold->index_of(f);
        v.cx = 0;
        v.stack.push_back(v);
        v.fold = f;
    }
    v.top = v.left = 0;
}
//}}}
//{{{  move_to
void Editor::move_to(int y, int x, bool extend)
{
    if (extend && !v.sel) { v.sel = true; v.sy = v.cy; v.sx = v.cx; }
    if (!extend) v.sel = false;
    v.cy = y;
    v.cx = x;
    if (v.cy >= (int)v.fold->kids.size()) v.cy = (int)v.fold->kids.size() - 1;
    if (v.cy < 0) v.cy = 0;
    if (v.cx > disp_len(cur())) v.cx = disp_len(cur());
    if (v.cx < 0) v.cx = 0;
}
//}}}
//}}}
//{{{  clipboard
// Every cut or copy is also put on the system clipboard, and the terminal's
// own paste (Ctrl+V in Windows Terminal) arrives as a bracketed paste.  If
// it is what we last put there we paste our own buffer, which keeps folds
// exactly; otherwise it came from another program and is read as lines.
//{{{  norm_paste
// terminals turn newlines into CRs on paste
static ustr norm_paste(const ustr &t)
{
    ustr r;
    for (size_t i = 0; i < t.size(); i++)
    {
        if (t[i] != '\r') { r += t[i]; continue; }
        r += U'\n';
        if (i + 1 < t.size() && t[i + 1] == '\n') i++;
    }
    while (!r.empty() && r.back() == '\n') r.pop_back();
    return r;
}
//}}}
//{{{  mirror
void Editor::mirror(const Paste *p)
{
    ustr text = p->inline_text ? p->lines[0]->text : write_lines(p->lines, *v.doc->lang);
    last_mirror = norm_paste(text);

    if (have_clip)
    {
        //clip.exe takes UTF-16 (a byte order mark would be pasted as a character)
        std::string u16;
        for (char32_t c : text)
        {
            if (c == '\n') { u16 += '\r'; u16 += '\0'; }
            if (is_raw_byte(c)) c = c - 0xDC00;
            if (c >= 0x10000)
            {
                char32_t x = c - 0x10000;
                char16_t hi = 0xD800 + (x >> 10), lo = 0xDC00 + (x & 0x3FF);
                u16 += (char)(hi & 0xFF); u16 += (char)(hi >> 8);
                u16 += (char)(lo & 0xFF); u16 += (char)(lo >> 8);
            }
            else { u16 += (char)(c & 0xFF); u16 += (char)(c >> 8); }
        }
        //clip.exe is slow to start, so run it in a detached grandchild
        pid_t pid = fork();
        if (pid == 0)
        {
            if (fork() == 0)
            {
                FILE *f = popen("clip.exe >/dev/null 2>&1", "w");
                if (f) { fwrite(u16.data(), 1, u16.size(), f); pclose(f); }
            }
            _exit(0);
        }
        if (pid > 0) waitpid(pid, nullptr, 0);
        return;
    }

    //not on Windows: ask the terminal to do it (OSC 52)
    static const char *b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string in = encode(text), out;
    for (size_t i = 0; i < in.size(); i += 3)
    {
        unsigned n = (unsigned char)in[i] << 16;
        if (i + 1 < in.size()) n |= (unsigned char)in[i + 1] << 8;
        if (i + 2 < in.size()) n |= (unsigned char)in[i + 2];
        out += b64[(n >> 18) & 63];
        out += b64[(n >> 12) & 63];
        out += i + 1 < in.size() ? b64[(n >> 6) & 63] : '=';
        out += i + 2 < in.size() ? b64[n & 63] : '=';
    }
    printf("\033]52;c;%s\a", out.c_str());
    fflush(stdout);
}
//}}}
//{{{  read_paste
// the text of a bracketed paste, after its start key has been read
ustr Editor::read_paste()
{
    ustr t;
    while (true)
    {
        int k = get_key();
        if (k == K(PASTE_END)) return t;
        if (k >= 0 && k < KB) t += (char32_t)k;
    }
}
//}}}
//{{{  paste_text
void Editor::paste_text(const ustr &raw)
{
    ustr text = norm_paste(raw);
    if (text.empty()) return;
    if (paste && text == last_mirror)
    {
        do_paste(false);
        return;
    }

    //from another program: text with no newlines goes inline, else lines
    std::unique_ptr<Paste> pp(new Paste);
    if (text.find(U'\n') == ustr::npos)
    {
        pp->lines.push_back(Line::make_text(text));
        pp->inline_text = true;
    }
    else
    {
        ParseResult r = parse_folds(text + U'\n', *v.doc->lang);
        pp->lines = std::move(r.root->kids);
        for (auto &l : pp->lines) if (l->fold) l->parent = nullptr;
    }
    push_paste(std::move(pp), false);
    last_mirror = text;
    do_paste(false);
}
//}}}
//}}}
//{{{  ui
//{{{  get_key
int Editor::get_key()
{
    wint_t ch;
    int r = get_wch(&ch);
    if (r == ERR) return -1;
    if (r == KEY_CODE_YES) return K((int)ch);
    return (int)ch;
}
//}}}
//{{{  handle
void Editor::handle(int k)
{
    msg.clear();
    prev_typing = typing_in;
    typing_in = nullptr;

    std::string kn;
    if (k >= KB) { const char *n = keyname(k - KB); if (n) kn = n; }
    int rows = LINES - 2;
    int n = (int)v.fold->kids.size();
    int len = disp_len(cur());

    //{{{  movement, shifted to select
    bool shift = k == K(KEY_SLEFT) || k == K(KEY_SRIGHT) || k == K(KEY_SR) || k == K(KEY_SF) ||
                 k == K(KEY_SHOME) || k == K(KEY_SEND) || kn == "kUP2" || kn == "kDN2";
    if (k == K(KEY_LEFT) || k == K(KEY_SLEFT))
    {
        v.want = -1;
        if (v.cx > 0) move_to(v.cy, v.cx - 1, shift);
        else if (v.cy > 0) move_to(v.cy - 1, disp_len(v.fold->kids[v.cy - 1].get()), shift);
        return;
    }
    if (k == K(KEY_RIGHT) || k == K(KEY_SRIGHT))
    {
        v.want = -1;
        if (v.cx < len) move_to(v.cy, v.cx + 1, shift);
        else if (v.cy + 1 < n) move_to(v.cy + 1, 0, shift);
        return;
    }
    if (k == K(KEY_UP) || k == K(KEY_DOWN) || k == K(KEY_SR) || k == K(KEY_SF) || kn == "kUP2" || kn == "kDN2")
    {
        if (v.want < 0) v.want = v.cx;
        int dy = (k == K(KEY_UP) || k == K(KEY_SR) || kn == "kUP2") ? -1 : 1;
        move_to(v.cy + dy, v.want, shift);
        return;
    }
    if (k == K(KEY_SHOME)) { move_to(v.cy, 0, true); return; }
    if (k == K(KEY_SEND))  { move_to(v.cy, len, true); return; }
    v.want = -1;
    if (k == K(KEY_NPAGE) || kn == "kNXT5") { move_to(v.cy + rows, v.cx, false); v.top += rows; return; }
    if (kn == "kPRV5") { move_to(v.cy - rows, v.cx, false); v.top = std::max(0, v.top - rows); return; }
    if (kn == "kHOM5") { move_to(0, 0, false); return; }
    if (kn == "kEND5") { move_to(n - 1, disp_len(v.fold->kids[n - 1].get()), false); return; }
    if (k == K(KEY_END)) { move_to(v.cy, len, false); return; }
    //}}}
    //{{{  folds
    if (k == K(KEY_HOME))  { enter_fold(false); return; }
    if (kn == "kLFT5")     { enter_fold(true); return; }
    if (k == K(KEY_PPAGE) || kn == "kRIT5") { exit_fold(); return; }
    if (k == CTRL('n'))    { create_fold(); return; }
    if (k == CTRL('u'))    { unfold(); return; }
    //}}}
    //{{{  editing
    if (k == '\r' || k == '\n' || k == K(KEY_ENTER)) { key_enter(); return; }
    if (k == '\t')                                    { key_tab(); return; }
    if (k == K(KEY_DC))                               { key_delete(); return; }
    if (k == 127 || k == 8 || k == K(KEY_BACKSPACE))  { key_backspace(); return; }
    if (k == CTRL('y') || k == K(KEY_F(3)))           { cut_line(true); return; }
    if (k == CTRL('k') || k == K(KEY_F(4)))           { cut_line(false); return; }
    if (k == CTRL('b') || k == K(KEY_F(5)))           { do_paste(true); return; }
    if (k == CTRL('x'))                               { copy_sel(true); return; }
    if (k == CTRL('c'))                               { copy_sel(false); return; }
    if (k == CTRL('v'))                               { do_paste(false); return; }
    if (k == CTRL('q'))                               { paste_list(); return; }
    if (k == CTRL('r'))                               { indent_lines(1); return; }
    if (k == CTRL('e'))                               { indent_lines(-1); return; }
    if (k == CTRL('d'))                               { insert_date(); return; }
    if (k == CTRL('t'))
    {
        if (v.fold->parent) insert_text(v.fold->text);
        else msg = "not in a fold";
        return;
    }
    if (k == CTRL('a'))                               { insert_text(search); return; }
    if (k == CTRL('z'))                               { undo(); return; }
    //}}}
    //{{{  other
    if (k == CTRL('f'))     { find(true); return; }
    if (k == CTRL('g'))     { find(false); return; }
    if (k == CTRL('s'))     { save_doc(v.doc); return; }
    if (k == CTRL('l'))     { clearok(curscr, TRUE); return; }
    if (k == 27)            { v.sel = false; return; }
    if (k == K(KEY_F(1)))   { help(); return; }
    if (k == K(KEY_F(10)))  { quit_all(); return; }
    if (k == K(KEY_F(6)))   { macro.clear(); recording = true; msg = "recording keys (F7 stops)"; return; }
    if (k == K(KEY_F(7)))   { recording = false; msg = std::to_string(macro.size()) + " keys recorded"; return; }
    if (k == K(KEY_F(8)))
    {
        if (recording || playing) return;
        playing = true;
        for (int mk : macro) { handle(mk); clamp(); }
        playing = false;
        return;
    }
    if (k == K(KEY_RESIZE)) return;
    if (k == K(PASTE_START)) { paste_text(read_paste()); return; }
    //}}}

    if (k >= 32 && k < KB && k != 127) key_char((char32_t)k);
}
//}}}
//{{{  render
void Editor::render()
{
    erase();
    int rows = LINES, cols = COLS;
    if (rows < 3 || cols < 10) { refresh(); return; }
    int body = rows - 2;

    //{{{  scroll to the cursor
    if (v.cy < v.top) v.top = v.cy;
    if (v.cy >= v.top + body) v.top = v.cy - body + 1;
    int maxtop = std::max(0, (int)v.fold->kids.size() - body);
    if (v.top > maxtop) v.top = std::max(maxtop, v.cy - body + 1);
    if (v.top < 0) v.top = 0;
    if (v.cx < v.left) v.left = std::max(0, v.cx - cols / 4);
    if (v.cx >= v.left + cols) v.left = v.cx - cols + cols / 4;
    //}}}
    //{{{  header: file and the folds we are in
    std::string h = v.doc->name + (v.doc->modified ? " *" : "");
    std::vector<std::string> titles;
    for (Line *f = v.fold; f->parent; f = f->parent) titles.insert(titles.begin(), preview(f->text));
    for (auto &t : titles) h += "  >  " + t;
    ustr hu = decode(" " + h);
    if ((int)hu.size() > cols) hu = U"..." + hu.substr(hu.size() - cols + 3);
    hu.resize(cols, U' ');
    attron(A_REVERSE);
    mvaddnwstr(0, 0, (const wchar_t *)std::wstring(hu.begin(), hu.end()).c_str(), cols);
    attroff(A_REVERSE);
    //}}}
    //{{{  lines
    Sel s = get_sel();
    for (int r = 0; r < body; r++)
    {
        int i = v.top + r;
        if (i >= (int)v.fold->kids.size()) break;
        Line *l = v.fold->kids[i].get();
        ustr d = display(l);
        int ts = l->fold ? title_start(l) : 0;
        int plen = (int)cfg.fold_prefix.size();
        bool whole = s.any && !s.inl && i >= s.y1 && i <= s.y2;

        move(r + 1, 0);
        int end = std::min((int)d.size(), v.left + cols);
        for (int c = v.left; c < end; c++)
        {
            char32_t ch = d[c];
            attr_t a = A_NORMAL;
            short pair = 0;
            if (l->fold && c >= l->indent && c < l->indent + plen) { a |= A_BOLD; pair = 1; }
            else if (l->fold && c >= ts) a |= A_BOLD;
            if (whole || (s.any && s.inl && i == s.y1 && c >= s.x1 && c < s.x2)) a |= A_REVERSE;
            if (ch < 32 || (ch >= 127 && ch < 160) || is_raw_byte(ch)) { ch = '?'; a |= A_DIM; }
            wchar_t wc = (wchar_t)ch;
            attr_set(a, pair, nullptr);
            addnwstr(&wc, 1);
        }
        attr_set(A_NORMAL, 0, nullptr);
        if (whole && d.size() <= (size_t)v.left) { attron(A_REVERSE); addch(' '); attroff(A_REVERSE); }
    }
    //}}}
    //{{{  status line
    std::string st;
    if (!msg.empty()) st = msg;
    else
    {
        st = v.doc->lang->name;
        if (!v.stack.empty()) st += "  depth " + std::to_string(v.stack.size());
        if (recording) st += "  REC";
        if (paste) st += "  paste: " + paste->label;
    }
    std::string pos = "Ln " + std::to_string(v.cy + 1) + "/" + std::to_string(v.fold->kids.size()) +
                      "  Col " + std::to_string(v.cx + 1) + " ";
    ustr su = decode(st);
    int room = cols - (int)pos.size() - 1;
    if ((int)su.size() > room) su.resize(std::max(0, room));
    if (!msg.empty()) attron(A_BOLD);
    mvaddnwstr(rows - 1, 0, (const wchar_t *)std::wstring(su.begin(), su.end()).c_str(), (int)su.size());
    attroff(A_BOLD);
    mvaddstr(rows - 1, cols - (int)pos.size(), pos.c_str());
    //}}}

    move(1 + v.cy - v.top, v.cx - v.left);
    refresh();
}
//}}}
//{{{  prompt
bool Editor::prompt(const std::string &label, ustr &text)
{
    int pos = (int)text.size();
    ustr lab = decode(label);
    while (true)
    {
        render();
        move(LINES - 1, 0);
        clrtoeol();
        attron(A_BOLD);
        addnwstr((const wchar_t *)std::wstring(lab.begin(), lab.end()).c_str(), (int)lab.size());
        attroff(A_BOLD);
        addnwstr((const wchar_t *)std::wstring(text.begin(), text.end()).c_str(), (int)text.size());
        move(LINES - 1, (int)lab.size() + pos);
        refresh();

        int k = get_key();
        if (k == '\r' || k == '\n' || k == K(KEY_ENTER)) return true;
        if (k == 27 || k == CTRL('c') || k == CTRL('g')) return false;
        if (k == K(KEY_LEFT) && pos > 0) pos--;
        else if (k == K(KEY_RIGHT) && pos < (int)text.size()) pos++;
        else if (k == K(KEY_HOME)) pos = 0;
        else if (k == K(KEY_END)) pos = (int)text.size();
        else if (k == CTRL('u')) { text.clear(); pos = 0; }
        else if (k == K(PASTE_START))
        {
            ustr t = norm_paste(read_paste());
            t = t.substr(0, t.find(U'\n'));
            text.insert(pos, t);
            pos += (int)t.size();
        }
        else if ((k == 127 || k == 8 || k == K(KEY_BACKSPACE)) && pos > 0) text.erase(--pos, 1);
        else if (k == K(KEY_DC) && pos < (int)text.size()) text.erase(pos, 1);
        else if (k >= 32 && k < KB && k != 127) text.insert(pos++, 1, (char32_t)k);
    }
}
//}}}
//{{{  ask
// returns the answer character, or 0 for Esc
int Editor::ask(const std::string &question, const std::string &answers)
{
    std::string saved = msg;
    msg = question;
    while (true)
    {
        render();
        int k = get_key();
        if (k == 27 || k == CTRL('c') || k == CTRL('g')) { msg = saved; return 0; }
        if (k > 0 && k < 128 && answers.find((char)tolower(k)) != std::string::npos)
        {
            msg = saved;
            return tolower(k);
        }
    }
}
//}}}
//{{{  choose
int Editor::choose(const std::string &title, const std::vector<std::string> &items, int sel)
{
    int top = 0;
    while (true)
    {
        erase();
        int body = LINES - 2;
        if (sel < top) top = sel;
        if (sel >= top + body) top = sel - body + 1;
        attron(A_REVERSE);
        mvaddstr(0, 0, (" " + title + std::string(std::max(0, COLS), ' ')).substr(0, COLS).c_str());
        attroff(A_REVERSE);
        for (int r = 0; r < body && top + r < (int)items.size(); r++)
        {
            if (top + r == sel) attron(A_REVERSE);
            mvaddstr(r + 1, 0, (" " + items[top + r]).substr(0, COLS).c_str());
            if (top + r == sel) attroff(A_REVERSE);
        }
        mvaddstr(LINES - 1, 0, "Up/Down to choose, Enter to select, Esc to cancel");
        refresh();

        int k = get_key();
        int n = (int)items.size();
        if (k == K(KEY_UP) && sel > 0) sel--;
        else if (k == K(KEY_DOWN) && sel + 1 < n) sel++;
        else if (k == K(KEY_PPAGE)) sel = std::max(0, sel - body);
        else if (k == K(KEY_NPAGE)) sel = std::min(n - 1, sel + body);
        else if (k == '\r' || k == '\n' || k == K(KEY_ENTER)) return sel;
        else if (k == 27 || k == CTRL('c') || k == CTRL('g')) return -1;
    }
}
//}}}
//{{{  paste_list
// choose an earlier paste buffer (Ctrl+Q, "restore")
void Editor::paste_list()
{
    if (pastes.empty()) { msg = "no paste buffers yet"; return; }
    std::vector<std::string> items;
    int sel = 0;
    for (int i = (int)pastes.size() - 1; i >= 0; i--)
    {
        if (pastes[i].get() == paste) sel = (int)items.size();
        items.push_back(pastes[i]->label);
    }
    int c = choose("Paste buffers (newest first)", items, sel);
    if (c < 0) return;
    paste = pastes[pastes.size() - 1 - c].get();
    mirror(paste);
    msg = "paste buffer: " + paste->label;
}
//}}}
//{{{  help
void Editor::help()
{
    static const char *text[] = {
        "foldit keys",
        "",
        "  Home / Ctrl+Left    enter fold, or open the file named in quotes on the line",
        "                      (Home elsewhere goes to the start of the line)",
        "  PgUp / Ctrl+Right   exit fold; at the top of a file go back, or quit",
        "  Ctrl+N              create fold (from the selected lines, or an empty one)",
        "  Ctrl+U              unfold",
        "",
        "  F3 / Ctrl+Y         cut line          Ctrl+X   cut selection",
        "  F4 / Ctrl+K         copy line         Ctrl+C   copy selection",
        "  F5 / Ctrl+B         paste line        Ctrl+V   paste",
        "  Ctrl+Q              choose an earlier paste buffer",
        "  (cuts and copies also go to the Windows clipboard; paste with Ctrl+V)",
        "  Shift+arrows        select            Esc      clear selection",
        "",
        "  Ctrl+F / Ctrl+G     find / find next (in this fold and folds inside it)",
        "  Ctrl+R / Ctrl+E     indent / outdent lines (selected, or all in fold)",
        "  Ctrl+D  date        Ctrl+T  fold title        Ctrl+A  find string",
        "  Ctrl+Z              undo",
        "  F6 / F7 / F8        record keys / stop / play back",
        "",
        "  Ctrl+S              save              F10      quit",
        "  PgDn, Ctrl+PgUp/PgDn  page            Ctrl+Home/End  top / bottom of fold",
        "",
        "press any key",
    };
    erase();
    int r = 0;
    for (const char *t : text) mvaddstr(r++, 1, t);
    refresh();
    get_key();
}
//}}}
//{{{  quit_all
void Editor::quit_all()
{
    for (auto &d : docs)
    {
        if (!d->modified) continue;
        int a = ask("Save changes to " + d->name + "? (y/n)", "yn");
        if (!a) return;
        if (a == 'y' && !save_doc(d.get())) return;
    }
    quit = true;
}
//}}}
//}}}

//{{{  open_file
bool Editor::open_file(const std::string &path, std::string &err)
{
    Doc *d = load_doc(path, err);
    if (!d) return false;
    v.doc  = d;
    v.fold = d->root.get();
    return true;
}
//}}}
//{{{  run
void Editor::run()
{
    initscr();
    raw();
    noecho();
    nonl();
    keypad(stdscr, TRUE);
    define_key("\033[200~", PASTE_START);
    define_key("\033[201~", PASTE_END);
    have_clip = system("command -v clip.exe >/dev/null 2>&1") == 0;
    printf("\033[?2004h");     //bracketed paste on
    fflush(stdout);
    set_escdelay(25);
    if (has_colors())
    {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_CYAN, -1);
    }

    while (!quit)
    {
        clamp();
        render();
        int k = get_key();
        if (k < 0) continue;
        if (recording && k != K(KEY_F(6)) && k != K(KEY_F(7)) && k != K(KEY_F(8))) macro.push_back(k);
        handle(k);
    }
    printf("\033[?2004l");
    fflush(stdout);
    endwin();
}
//}}}
