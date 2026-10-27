//{{{}}}
// fold.cpp : the fold tree, and reading/writing it as marked-up text

#include "fold.h"

//{{{  Line
LinePtr Line::make_text(const ustr &t)
{
    LinePtr l(new Line);
    l->text = t;
    return l;
}

LinePtr Line::make_fold(const ustr &title, int indent)
{
    LinePtr l(new Line);
    l->fold   = true;
    l->text   = title;
    l->indent = indent;
    return l;
}

LinePtr Line::clone() const
{
    LinePtr c(new Line);
    c->fold   = fold;
    c->text   = text;
    c->indent = indent;
    for (auto &k : kids)
    {
        LinePtr kc = k->clone();
        if (kc->fold) kc->parent = c.get();
        c->kids.push_back(std::move(kc));
    }
    return c;
}

int Line::index_of(const Line *kid) const
{
    for (size_t i = 0; i < kids.size(); i++)
        if (kids[i].get() == kid) return (int)i;
    return -1;
}

bool Line::contains(const Line *f) const
{
    for (; f; f = f->parent)
        if (f == this) return true;
    return false;
}

void Line::ensure_not_empty()
{
    if (kids.empty()) kids.push_back(make_text());
}

void Line::adopt(LinePtr &l, int at)
{
    if (l->fold) l->parent = this;
    kids.insert(kids.begin() + at, std::move(l));
}

LinePtr Line::remove(int at)
{
    LinePtr l = std::move(kids[at]);
    kids.erase(kids.begin() + at);
    if (l->fold) l->parent = nullptr;
    return l;
}
//}}}
//{{{  parse_folds
ParseResult parse_folds(const ustr &content, const Language &lang)
{
    ParseResult r;
    r.root = Line::make_fold(U"", 0);
    Line *cur = r.root.get();
    int full = 0;    //absolute indent of the current fold

    ustr start = lang.start_mark();
    ustr bare  = lang.scom + lang.sfold;     //start mark whose title spaces were trimmed
    ustr end   = lang.lazy_end_mark();
    bool marks = lang.has_marks();

    size_t p = 0, n = content.size();
    while (p < n)
    {
        //{{{  next raw line
        size_t e = content.find(U'\n', p);
        if (e == ustr::npos) e = n;
        ustr buf = content.substr(p, e - p);
        p = e + 1;
        if (!buf.empty() && buf.back() == '\r') buf.pop_back();
        //}}}
        //{{{  make relative to current fold
        buf = expand_tabs(buf, lang.tab);
        if (is_blank(buf))
        {
            cur->kids.push_back(Line::make_text());
            continue;
        }
        int indent = leading_spaces(buf);
        bool under = indent < full;
        if (under)
            r.under_indented++;     //kept as is; save will indent it
        else
        {
            indent -= full;
            buf = buf.substr(full);
        }
        //}}}

        if (marks)
        {
            ustr trimmed = buf;
            rtrim(trimmed);
            //{{{  start of fold
            bool is_start = starts_with_at(buf, indent, start);
            if (!is_start)
            {
                ustr rest = trimmed.substr(indent);
                is_start = rest == bare || (!lang.ecom.empty() && rest == bare + lang.ecom);
            }
            if (is_start)
            {
                ustr title = indent + start.size() <= trimmed.size() ? trimmed.substr(indent + start.size()) : ustr();
                if (!lang.ecom.empty() && ends_with(title, lang.ecom))
                    title.resize(title.size() - lang.ecom.size());
                rtrim(title);
                LinePtr f = Line::make_fold(title, under ? 0 : indent);
                Line *fp = f.get();
                cur->adopt(f, (int)cur->kids.size());
                full += fp->indent;
                cur = fp;
                continue;
            }
            //}}}
            //{{{  end of fold
            if (starts_with_at(buf, indent, end))
            {
                if (!cur->parent)
                    r.extra_ends++;     //keep it as text so nothing is lost
                else
                {
                    full -= cur->indent;
                    cur->ensure_not_empty();
                    cur = cur->parent;
                    continue;
                }
            }
            //}}}
        }

        cur->kids.push_back(Line::make_text(buf));
    }

    for (; cur->parent; cur = cur->parent)
    {
        r.unclosed++;
        cur->ensure_not_empty();
    }
    r.root->ensure_not_empty();
    return r;
}
//}}}
//{{{  write
static void write_line(const Line &l, int base, const Language &lang, ustr &out);

static void write_kids(const Line &fold, int base, const Language &lang, ustr &out)
{
    for (auto &k : fold.kids) write_line(*k, base, lang, out);
}

static void write_line(const Line &l, int base, const Language &lang, ustr &out)
{
    ustr pad(base, U' ');
    if (!l.fold)
    {
        ustr t = l.text;
        rtrim(t);
        if (!t.empty()) out += pad + t;
        out += U'\n';
        return;
    }
    int in = base + l.indent;
    if (!lang.has_marks())
    {
        write_kids(l, in, lang, out);       //no markers: folds are just flattened
        return;
    }
    ustr fpad(in, U' ');
    ustr title = l.text;
    rtrim(title);
    out += fpad + lang.start_mark() + title + lang.ecom + U'\n';
    write_kids(l, in, lang, out);
    out += fpad + lang.scom + lang.efold + lang.ecom + U'\n';
}

ustr write_folds(const Line &fold, const Language &lang)
{
    ustr out;
    write_kids(fold, 0, lang, out);
    return out;
}

ustr write_lines(const LineList &lines, const Language &lang)
{
    ustr out;
    for (auto &l : lines) write_line(*l, 0, lang, out);
    return out;
}
//}}}
