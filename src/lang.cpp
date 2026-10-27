//{{{}}}
// lang.cpp : languages and foldit.ini configuration

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <climits>
#include "lang.h"

//{{{  string helpers
static std::string lc(std::string s)
{
    for (auto &c : s) c = tolower((unsigned char)c);
    return s;
}

static std::string strip(const std::string &s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static bool file_exists(const std::string &p)
{
    return !p.empty() && access(p.c_str(), R_OK) == 0;
}
//}}}
//{{{  Ini
bool Ini::load(const std::string &path)
{
    std::ifstream f(path);
    return f && parse(f);
}

bool Ini::load_string(const std::string &text)
{
    std::istringstream f(text);
    return parse(f);
}

bool Ini::parse(std::istream &f)
{
    std::string line;
    sections.clear();
    while (std::getline(f, line))
    {
        line = strip(line);
        if (line.empty() || line[0] == ';') continue;
        if (line[0] == '[')
        {
            size_t e = line.find(']');
            sections.push_back({line.substr(1, e == std::string::npos ? std::string::npos : e - 1), {}});
            continue;
        }
        if (sections.empty()) continue;
        size_t eq = line.find('=');
        std::string k = strip(line.substr(0, eq));
        std::string v = eq == std::string::npos ? "" : strip(line.substr(eq + 1));
        if (v.size() >= 2 && v.front() == '"' && v.back() == '"') v = v.substr(1, v.size() - 2);
        sections.back().kv.push_back({k, v});
    }
    return true;
}

const Ini::Section *Ini::find(const std::string &sec) const
{
    for (auto &s : sections)
        if (lc(s.name) == lc(sec)) return &s;
    return nullptr;
}

std::string Ini::get(const std::string &sec, const std::string &key, const std::string &def) const
{
    const Section *s = find(sec);
    if (!s) return def;
    for (auto &kv : s->kv)
        if (lc(kv.first) == lc(key)) return kv.second;
    return def;
}

int Ini::get_int(const std::string &sec, const std::string &key, int def) const
{
    std::string v = get(sec, key, "");
    if (v.empty()) return def;
    return atoi(v.c_str());
}
//}}}
//{{{  Config::load
static const char *builtin_ini =
    "[Languages]\nC++=\nC=\nShell=\nLua=\nHTML=\nINI=\n"
    "[C++]\nStartComment=//\nTab=8\nFileExt=cpp,cc,cxx,h,hpp,js,ts,java,cs,go,rs\nStartFoldMark={{{\nEndFoldMark=}}}\n"
    "[C]\nStartComment=/*\nEndComment=*/\nTab=8\nFileExt=c\nStartFoldMark={{{\nEndFoldMark=}}}\n"
    "[Shell]\nStartComment=#\nTab=8\nFileExt=sh,bash,py,pl,rb,mk,toml,yaml,yml,conf\nStartFoldMark={{{\nEndFoldMark=}}}\n"
    "[Lua]\nStartComment=--\nTab=2\nFileExt=lua,sql\nStartFoldMark={{{\nEndFoldMark=}}}\n"
    "[HTML]\nStartComment=<!--\nEndComment=-->\nTab=2\nFileExt=html,htm,xml,md\nStartFoldMark={{{\nEndFoldMark=}}}\n"
    "[INI]\nStartComment=;\nTab=8\nFileExt=ini\nStartFoldMark={{{\nEndFoldMark=}}}\n";

void Config::load()
{
    std::vector<std::string> cands;
    if (const char *e = getenv("FOLDIT_INI")) cands.push_back(e);
    if (const char *x = getenv("XDG_CONFIG_HOME")) cands.push_back(std::string(x) + "/foldit/foldit.ini");
    if (const char *h = getenv("HOME")) cands.push_back(std::string(h) + "/.config/foldit/foldit.ini");
    char exe[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (n > 0)
    {
        exe[n] = 0;
        std::string d(exe);
        d = d.substr(0, d.rfind('/'));
        cands.push_back(d + "/foldit.ini");
    }

    Ini ini;
    for (auto &c : cands)
        if (file_exists(c) && ini.load(c)) { ini_path = c; load_from(ini); return; }

    //nothing found - use the built in definitions
    ini.load_string(builtin_ini);
    load_from(ini);
}
//}}}
//{{{  Config::load_from
void Config::load_from(const Ini &ini)
{
    langs.clear();
    if (const Ini::Section *names = ini.find("Languages"))
    {
        for (auto &kv : names->kv)
        {
            Language L;
            L.name  = kv.first;
            L.scom  = decode(ini.get(L.name, "StartComment"));
            L.ecom  = decode(ini.get(L.name, "EndComment"));
            L.sfold = decode(ini.get(L.name, "StartFoldMark"));
            L.efold = decode(ini.get(L.name, "EndFoldMark"));
            L.ref   = decode(ini.get(L.name, "FileRef"));
            L.tab   = ini.get_int(L.name, "Tab", 8);
            std::string ext = ini.get(L.name, "FileExt");
            for (auto &c : ext) if (c == ';' || c == ' ') c = ',';
            size_t p = 0;
            while (p <= ext.size())
            {
                size_t q = ext.find(',', p);
                if (q == std::string::npos) q = ext.size();
                std::string e = strip(ext.substr(p, q - p));
                while (!e.empty() && (e[0] == '.' || e[0] == '*')) e.erase(0, 1);
                if (!e.empty()) L.exts.push_back(lc(e));
                p = q + 1;
            }
            if (L.name != "Plain Text") langs.push_back(L);
        }
    }

    Language plain;
    plain.name  = "Plain Text";
    plain.sfold = U"{{{";
    plain.efold = U"}}}";
    langs.push_back(plain);

    std::string prefix = ini.get("Defaults", "FoldPrefix", "...");
    fold_prefix    = decode(prefix);
    date_tag       = decode(ini.get("Defaults", "DateTag", ""));
    use_file_refs  = ini.get_int("Defaults", "UseOpenFileRefs", 1) != 0;
    del_fold_tails = ini.get_int("Defaults", "DelFoldTails", 0) != 0;
    paste_len      = ini.get_int("Defaults", "PasteBufferLen", 100);
    if (paste_len < 2) paste_len = 2;
    default_lang   = ini.get("Defaults", "DefaultLanguage", "Plain Text");
}
//}}}
//{{{  Config::by_name
const Language *Config::by_name(const std::string &name) const
{
    for (auto &L : langs)
        if (lc(L.name) == lc(name)) return &L;
    return nullptr;
}
//}}}
//{{{  Config::detect
// find a proof string at the start of a line or after white space
static size_t find_word(const ustr &buf, const ustr &proof, size_t limit)
{
    if (proof.empty()) return ustr::npos;
    size_t at = 0;
    while ((at = buf.find(proof, at)) != ustr::npos && at < limit)
    {
        if (at == 0 || buf[at - 1] <= ' ') return at;
        at++;
    }
    return ustr::npos;
}

const Language *Config::detect(const ustr &head, const std::string &ext) const
{
    //{{{  <SC><SF>name<EF><EC> near the start
    for (auto &L : langs)
        if (L.has_marks() && !L.efold.empty())
            if (find_word(head, L.scom + L.sfold + decode(L.name) + L.efold + L.ecom, 64) != ustr::npos)
                return &L;
    //}}}
    //{{{  <SC><SF><EF><EC> near the start
    for (auto &L : langs)
        if (L.has_marks() && !L.efold.empty())
            if (find_word(head, L.scom + L.sfold + L.efold + L.ecom, 64) != ustr::npos)
                return &L;
    //}}}
    //{{{  a fold start anywhere in the head
    for (auto &L : langs)
        if (L.has_marks() && !L.scom.empty())
            if (find_word(head, L.start_mark(), head.size()) != ustr::npos)
                return &L;
    //}}}
    //{{{  file extension
    std::string e = lc(ext);
    if (!e.empty())
        for (auto &L : langs)
            for (auto &x : L.exts)
                if (x == e) return &L;
    //}}}
    //{{{  a comment start anywhere in the head
    for (auto &L : langs)
        if (!L.scom.empty() && L.scom[0] > ' ')
            if (find_word(head, L.scom, head.size()) != ustr::npos)
                return &L;
    //}}}
    return by_name(default_lang) ? by_name(default_lang) : &langs.back();
}
//}}}
