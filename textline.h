//{{{}}}

#ifndef __TEXTLINE_H__
#define __TEXTLINE_H__

//{{{  includes
#ifndef __LINE_H__
#include "line.h"
#endif
//}}}

//{{{  CTextLine
class CTextLine : public CLine
{
  //{{{  constructors
  public:
  
    CTextLine (const char *Buf = NULL);
    CTextLine (const CTextLine &Source);
  //}}}
  //{{{  operators
  
  public:
  
    CTextLine& operator=(const CTextLine &Source);
  //}}}
  //{{{  access interface
  
  public:
  
    BOOL IsTextP () const {return TRUE;};
    void Render (CString &Buf) const;
  //}}}
};
//}}}

#endif

