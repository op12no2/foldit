//{{{}}}
// text.h : unicode text helpers

#ifndef TEXT_H
#define TEXT_H

#include <string>

using ustr = std::u32string;

//{{{  conversion
// UTF-8 <-> UTF-32.  Bytes that are not valid UTF-8 (e.g. old cp1252 files)
// decode to U+DC80..U+DCFF and encode back to the same byte, so a load/save
// cycle never changes them.
ustr        decode(const std::string &s);
std::string encode(const ustr &s);
inline bool is_raw_byte(char32_t c) { return c >= 0xDC80 && c <= 0xDCFF; }
//}}}
//{{{  helpers
ustr  lower(const ustr &s);
int   leading_spaces(const ustr &s);
void  rtrim(ustr &s);
ustr  trim(const ustr &s);
bool  is_blank(const ustr &s);
bool  starts_with_at(const ustr &s, size_t at, const ustr &what);
bool  ends_with(const ustr &s, const ustr &what);
ustr  expand_tabs(const ustr &s, int tab);
//}}}

#endif
