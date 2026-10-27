//{{{}}}
// fold.h : the fold tree, and reading/writing it as marked-up text

#ifndef FOLD_H
#define FOLD_H

#include <memory>
#include <string>
#include <vector>
#include "text.h"
#include "lang.h"

//{{{  Line
// A document is a tree of lines.  A text line holds its text (including any
// indentation relative to the fold it lives in).  A fold line holds its title,
// its indent relative to its parent and the lines inside it.  The document
// root is a fold with no parent whose title is unused.
struct Line;
using LinePtr  = std::unique_ptr<Line>;
using LineList = std::vector<LinePtr>;

struct Line
{
    bool     fold   = false;
    ustr     text;              //body of a text line or title of a fold
    int      indent = 0;        //fold only: indent relative to parent
    Line    *parent = nullptr;  //fold only
    LineList kids;              //fold only

    static LinePtr make_text(const ustr &t = ustr());
    static LinePtr make_fold(const ustr &title, int indent);

    LinePtr clone() const;                  //deep copy, parent left null
    int     index_of(const Line *kid) const;
    bool    contains(const Line *f) const;  //f is this or below this
    void    ensure_not_empty();             //a fold always shows at least one line
    void    adopt(LinePtr &l, int at);      //insert at index, fixing parent
    LinePtr remove(int at);
};
//}}}
//{{{  reading and writing
struct ParseResult
{
    LinePtr root;
    int     under_indented = 0;   //lines less indented than their fold
    int     extra_ends     = 0;   //end markers with no matching start
    int     unclosed       = 0;   //folds still open at end of file
};

ParseResult parse_folds(const ustr &content, const Language &lang);

// write a fold's contents (not the fold's own markers) as text with markers
ustr write_folds(const Line &fold, const Language &lang);

// write a run of lines (e.g. a selection) as text with markers
ustr write_lines(const LineList &lines, const Language &lang);
//}}}

#endif
