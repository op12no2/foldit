//{{{}}}
// editor.h : documents, the view and the terminal editor

#ifndef EDITOR_H
#define EDITOR_H

#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include "fold.h"

//{{{  Doc
struct Doc
{
    struct Snapshot
    {
        LinePtr          root;
        std::vector<int> path;      //fold the view was in
        int              cy, cx, top;
    };

    std::string     path;           //absolute
    std::string     name;           //as shown in the header
    const Language *lang = nullptr;
    LinePtr         root;
    bool            modified = false;
    bool            crlf     = false;
    bool            exists   = false;
    time_t          mtime    = 0;
    std::vector<Snapshot> undo;
};
//}}}
//{{{  View
// There is one view; it shows one fold at a time.  Entering a fold, or a
// file reference, pushes where we were so exiting returns to the same place.
struct Place
{
    Doc  *doc  = nullptr;
    Line *fold = nullptr;
    int   cy = 0, cx = 0, top = 0;
};

struct View : Place
{
    int   left = 0;                 //horizontal scroll
    int   want = -1;                //column to aim for moving up/down
    bool  sel  = false;             //selection from (sy,sx) to cursor
    int   sy = 0, sx = 0;
    std::vector<Place> stack;
};
//}}}
//{{{  Paste
// A paste buffer is either some characters from within one line (inline)
// or a run of whole lines (which may be folds).
struct Paste
{
    LineList    lines;
    bool        inline_text = false;
    std::string label;
};
//}}}
//{{{  Editor
class Editor
{
public:
    explicit Editor(Config &cfg) : cfg(cfg) {}
    bool open_file(const std::string &path, std::string &err);
    void run();

private:
    struct Sel { bool any = false, inl = false; int y1 = 0, x1 = 0, y2 = 0, x2 = 0; };

    Config &cfg;
    std::vector<std::unique_ptr<Doc>>  docs;
    View        v;
    std::deque<std::unique_ptr<Paste>> pastes;
    Paste      *paste = nullptr;                   //current paste buffer
    ustr        search;
    Doc        *find_doc = nullptr;
    std::vector<int> find_path;                    //fold find-first was done in
    std::string msg;
    bool        quit = false;
    bool        recording = false, playing = false;
    std::vector<int> macro;
    ustr        last_mirror;                       //normalised text last put on the clipboard
    bool        have_clip = false;                 //clip.exe available
    Line       *typing_in = nullptr;               //for undo coalescing: line typed in
    Line       *prev_typing = nullptr;             //  by this key and the previous one

    //{{{  documents
    Doc  *load_doc(const std::string &path, std::string &err);
    bool  save_doc(Doc *d);
    bool  release_doc(Doc *d);
    bool  doc_in_stack(Doc *d) const;
    //}}}
    //{{{  view
    Line *cur() { return v.fold->kids[v.cy].get(); }
    void  clamp();
    bool  in_use(const Line *f) const;
    //}}}
    //{{{  geometry
    int  title_start(const Line *l) const { return l->indent + (int)cfg.fold_prefix.size() + 2; }
    int  disp_len(const Line *l) const { return l->fold ? title_start(l) + (int)l->text.size() : (int)l->text.size(); }
    ustr display(const Line *l) const;
    //}}}
    //{{{  undo
    void begin_edit(bool typing = false);
    void undo();
    //}}}
    //{{{  selection and paste buffers
    Sel  get_sel();
    bool delete_sel();
    void copy_sel(bool cut);
    void push_paste(std::unique_ptr<Paste> p, bool to_clipboard = true);
    void do_paste(bool as_lines);
    //}}}
    //{{{  editing
    bool insert_text(const ustr &s, bool typing = false);
    void key_char(char32_t c);
    void key_tab();
    void key_enter();
    void key_delete();
    void key_backspace();
    void cut_line(bool cut);
    void create_fold();
    void unfold();
    void indent_lines(int dir);
    void insert_date();
    //}}}
    //{{{  navigation
    void enter_fold(bool explicit_key);
    void exit_fold();
    bool file_ref(std::string &path);
    void open_ref(const std::string &path, bool create);
    void find(bool first);
    void goto_fold(Line *target);
    void move_to(int y, int x, bool extend);
    //}}}
    //{{{  clipboard
    void mirror(const Paste *p);
    ustr read_paste();
    void paste_text(const ustr &raw);
    //}}}
    //{{{  ui
    int  get_key();
    void handle(int k);
    void render();
    bool prompt(const std::string &label, ustr &text);
    int  ask(const std::string &question, const std::string &answers);
    int  choose(const std::string &title, const std::vector<std::string> &items, int initial);
    void paste_list();
    void help();
    void quit_all();
    //}}}
};
//}}}

#endif
