//{{{}}}
// lang.h : languages and foldit.ini configuration

#ifndef LANG_H
#define LANG_H

#include <istream>
#include <string>
#include <vector>
#include "text.h"

//{{{  Language
// how folds are marked in a given kind of file.  A fold start line is
// <indent><SCom><SFold>"  "<title><ECom> and an end line <indent><SCom><EFold><ECom>.
struct Language
{
    std::string name;
    ustr scom, ecom;            //comment start/end
    ustr sfold, efold;          //fold marks, usually {{{ and }}}
    ustr ref;                   //file reference prefix (informational)
    int  tab = 8;               //tab width used to expand tabs on load
    int  indent = 0;            //Tab key width, 0 for the default
    std::vector<std::string> exts;  //extensions without dot, used when no markers found

    bool has_marks() const { return !sfold.empty() && sfold[0] > ' '; }
    ustr start_mark() const { return scom + sfold + U"  "; }
    ustr lazy_end_mark() const { return scom + efold; }
};
//}}}
//{{{  Ini
// minimal .ini reader keeping section and key order; keys are case-insensitive
struct Ini
{
    struct Section { std::string name; std::vector<std::pair<std::string, std::string>> kv; };
    std::vector<Section> sections;

    bool load(const std::string &path);
    bool load_string(const std::string &text);
    bool parse(std::istream &f);
    const Section *find(const std::string &sec) const;
    std::string get(const std::string &sec, const std::string &key, const std::string &def = "") const;
    int get_int(const std::string &sec, const std::string &key, int def) const;
};
//}}}
//{{{  Config
struct Config
{
    std::string ini_path;               //where it was loaded from ("" if built-in)
    std::vector<Language> langs;        //"Plain Text" is always last
    std::string default_lang = "Plain Text";
    ustr fold_prefix = U"...";
    ustr date_tag    = U"";
    bool use_file_refs  = true;
    bool del_fold_tails = false;
    int  paste_len      = 100;
    int  indent         = 2;    //Tab key width

    void load();                         //search the usual places
    void load_from(const Ini &ini);
    const Language *by_name(const std::string &name) const;
    const Language *detect(const ustr &head, const std::string &ext) const;
};
//}}}

#endif
