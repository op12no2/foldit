//{{{}}}
// test_fold.cpp : parse/write tests for the fold tree
//
// run with "make test"; extra file arguments are round-trip checked too.

#include <cstdio>
#include <fstream>
#include <sstream>
#include <glob.h>
#include "fold.h"

static int failures = 0;

#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); failures++; } } while (0)

//{{{  helpers
static std::string slurp(const std::string &path)
{
    std::ifstream f(path, std::ios::binary);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// what a save of an unedited file should produce: LF endings, tabs
// expanded, no trailing white space, final newline
static ustr normalise(const ustr &in, int tab)
{
    ustr out, line;
    size_t p = 0;
    while (p < in.size())
    {
        size_t e = in.find(U'\n', p);
        if (e == ustr::npos) e = in.size();
        line = in.substr(p, e - p);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        line = expand_tabs(line, tab);
        rtrim(line);
        out += line + U'\n';
        p = e + 1;
    }
    return out;
}

static Config config()
{
    Config c;
    Ini ini;
    ini.load("foldit.ini");
    c.load_from(ini);
    return c;
}
//}}}
//{{{  test_basic
static void test_basic(const Config &cfg)
{
    const Language *L = cfg.by_name("C++");
    ustr src = U"//{{{}}}\n"
               U"int a;\n"
               U"//{{{  outer\n"
               U"  //{{{  inner\n"
               U"  x\n"
               U"  //}}}\n"
               U"\n"
               U"//}}}\n"
               U"end\n";
    ParseResult r = parse_folds(src, *L);
    Line &root = *r.root;
    CHECK(root.kids.size() == 4);
    CHECK(!root.kids[0]->fold && root.kids[0]->text == U"//{{{}}}");
    Line &outer = *root.kids[2];
    CHECK(outer.fold && outer.text == U"outer" && outer.indent == 0);
    CHECK(outer.kids.size() == 2);
    Line &inner = *outer.kids[0];
    CHECK(inner.fold && inner.indent == 2 && inner.parent == &outer);
    CHECK(inner.kids.size() == 1 && inner.kids[0]->text == U"x");
    CHECK(write_folds(root, *L) == src);
    CHECK(r.under_indented == 0 && r.extra_ends == 0 && r.unclosed == 0);
}
//}}}
//{{{  test_end_comment
static void test_end_comment(const Config &cfg)
{
    const Language *L = cfg.by_name("HTML");
    ustr src = U"<!--{{{  head-->\n<p>\n<!--}}}-->\n";
    ParseResult r = parse_folds(src, *L);
    CHECK(r.root->kids.size() == 1);
    CHECK(r.root->kids[0]->text == U"head");
    CHECK(write_folds(*r.root, *L) == src);
}
//}}}
//{{{  test_malformed
static void test_malformed(const Config &cfg)
{
    const Language *L = cfg.by_name("C++");
    ParseResult r = parse_folds(U"a\n//}}}\nb\n", *L);
    CHECK(r.extra_ends == 1);
    CHECK(r.root->kids.size() == 3);      //stray end mark kept as text

    r = parse_folds(U"//{{{  open\nx\n", *L);
    CHECK(r.unclosed == 1);
    CHECK(write_folds(*r.root, *L) == U"//{{{  open\nx\n//}}}\n");

    r = parse_folds(U"  //{{{  f\nx\n  //}}}\n", *L);
    CHECK(r.under_indented == 1);

    //start mark whose trailing spaces were stripped by another editor
    r = parse_folds(U"//{{{\nx\n//}}}\n", *L);
    CHECK(r.root->kids.size() == 1 && r.root->kids[0]->fold);
}
//}}}
//{{{  test_detect
static void test_detect(const Config &cfg)
{
    CHECK(cfg.detect(U"//{{{}}}\nint x;\n", "")->name == "C++");
    CHECK(cfg.detect(U"<!--{{{}}}-->\n", "")->name == "HTML");
    CHECK(cfg.detect(U"x\n  #{{{  stuff\n", "")->name == "Shell");
    CHECK(cfg.detect(U"print('hi')\n", "py")->name == "Shell");
    CHECK(cfg.detect(U"see http://example.com\n", "")->name == "Plain Text");
    CHECK(cfg.detect(U"--{{{Lua}}}\n", "")->name == "Lua");
}
//}}}
//{{{  test_utf8
static void test_utf8()
{
    std::string raw = "caf\xc3\xa9 \x93quoted\x94 \xff";
    ustr u = decode(raw);
    CHECK(u.size() == 15);
    CHECK(u[3] == 0xE9);
    CHECK(encode(u) == raw);
}
//}}}
//{{{  test_roundtrip
static int count_folds(const Line &f)
{
    int n = 0;
    for (auto &k : f.kids)
        if (k->fold) n += 1 + count_folds(*k);
    return n;
}

static void test_roundtrip(const Config &cfg, const std::string &path)
{
    ustr src = decode(slurp(path));
    std::string ext = path.substr(path.rfind('.') + 1);
    const Language *L = cfg.detect(src.substr(0, 4096), ext);
    ParseResult r = parse_folds(src, *L);
    ustr want = normalise(src, L->tab);
    ustr got  = write_folds(*r.root, *L);
    if (src.find(L->start_mark()) != ustr::npos && count_folds(*r.root) == 0)
    {
        printf("FAIL no folds found in %s\n", path.c_str());
        failures++;
    }
    if (got != want)
    {
        printf("FAIL round trip %s (%s)\n", path.c_str(), L->name.c_str());
        failures++;
    }
}
//}}}

int main(int argc, char **argv)
{
    Config cfg = config();
    test_basic(cfg);
    test_end_comment(cfg);
    test_malformed(cfg);
    test_detect(cfg);
    test_utf8();

    int n = 0;
    glob_t g;
    if (glob("mfc/*.cpp", 0, nullptr, &g) == 0)
    {
        for (size_t i = 0; i < g.gl_pathc; i++, n++) test_roundtrip(cfg, g.gl_pathv[i]);
        globfree(&g);
    }
    if (glob("mfc/*.h", 0, nullptr, &g) == 0)
    {
        for (size_t i = 0; i < g.gl_pathc; i++, n++) test_roundtrip(cfg, g.gl_pathv[i]);
        globfree(&g);
    }
    for (int i = 1; i < argc; i++, n++) test_roundtrip(cfg, argv[i]);

    printf("%s (%d round trips)\n", failures ? "FAILED" : "ok", n);
    return failures ? 1 : 0;
}
