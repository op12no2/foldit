//{{{}}}
// text.cpp : unicode text helpers

#include "text.h"

//{{{  decode
ustr decode(const std::string &s)
{
    ustr out;
    out.reserve(s.size());
    size_t i = 0, n = s.size();
    while (i < n)
    {
        unsigned char c = s[i];
        int len = 0;
        char32_t cp = 0;
        if (c < 0x80)                { out += c; i++; continue; }
        else if ((c & 0xE0) == 0xC0) { len = 2; cp = c & 0x1F; }
        else if ((c & 0xF0) == 0xE0) { len = 3; cp = c & 0x0F; }
        else if ((c & 0xF8) == 0xF0) { len = 4; cp = c & 0x07; }

        bool ok = len > 0 && i + len <= n;
        for (int k = 1; ok && k < len; k++)
        {
            unsigned char cc = s[i + k];
            if ((cc & 0xC0) != 0x80) ok = false;
            else cp = (cp << 6) | (cc & 0x3F);
        }
        //reject overlong forms, surrogates and out of range values
        if (ok && ((len == 2 && cp < 0x80) || (len == 3 && cp < 0x800) ||
                   (len == 4 && cp < 0x10000) || cp > 0x10FFFF ||
                   (cp >= 0xD800 && cp <= 0xDFFF)))
            ok = false;

        if (ok) { out += cp; i += len; }
        else    { out += (char32_t)(0xDC00 + c); i++; }
    }
    return out;
}
//}}}
//{{{  encode
std::string encode(const ustr &s)
{
    std::string out;
    out.reserve(s.size());
    for (char32_t c : s)
    {
        if (c < 0x80) out += (char)c;
        else if (is_raw_byte(c)) out += (char)(c - 0xDC00);
        else if (c < 0x800)
        {
            out += (char)(0xC0 | (c >> 6));
            out += (char)(0x80 | (c & 0x3F));
        }
        else if (c < 0x10000)
        {
            out += (char)(0xE0 | (c >> 12));
            out += (char)(0x80 | ((c >> 6) & 0x3F));
            out += (char)(0x80 | (c & 0x3F));
        }
        else
        {
            out += (char)(0xF0 | (c >> 18));
            out += (char)(0x80 | ((c >> 12) & 0x3F));
            out += (char)(0x80 | ((c >> 6) & 0x3F));
            out += (char)(0x80 | (c & 0x3F));
        }
    }
    return out;
}
//}}}
//{{{  helpers
ustr lower(const ustr &s)
{
    ustr r = s;
    for (auto &c : r)
        if (c >= 'A' && c <= 'Z') c += 32;
    return r;
}

int leading_spaces(const ustr &s)
{
    int i = 0;
    while (i < (int)s.size() && s[i] == ' ') i++;
    return i;
}

void rtrim(ustr &s)
{
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.pop_back();
}

ustr trim(const ustr &s)
{
    ustr r = s.substr(leading_spaces(s));
    rtrim(r);
    return r;
}

bool is_blank(const ustr &s)
{
    for (char32_t c : s)
        if (c != ' ' && c != '\t') return false;
    return true;
}

bool starts_with_at(const ustr &s, size_t at, const ustr &what)
{
    return at <= s.size() && s.compare(at, what.size(), what) == 0;
}

bool ends_with(const ustr &s, const ustr &what)
{
    return s.size() >= what.size() && s.compare(s.size() - what.size(), what.size(), what) == 0;
}

ustr expand_tabs(const ustr &s, int tab)
{
    if (tab <= 0 || s.find(U'\t') == ustr::npos) return s;
    ustr out;
    for (char32_t c : s)
    {
        if (c == '\t') out.append(tab - (out.size() % tab), U' ');
        else out += c;
    }
    return out;
}
//}}}
