//{{{}}}

#ifndef __LINE_H__
#define __LINE_H__

#ifndef _SENTENCE_H_
#include "sentence.h"
#endif

//{{{  CLine
class CLine : public CObject
{
//{{{  context
public: //data
    CSentence m_Body;  //the basic text for the single line, this will be the
          //actual line text for a text line and the fold title
          //for a fold line.
//}}}
//{{{  constructors
public: //constructors
    CLine(const char *Buf = NULL);
    CLine(const CLine& Source);
//}}}
//{{{  destructors
    ~CLine();
//}}}
//{{{  operators
public: //operators
    CLine& operator=(const CLine& Source);
    operator const char *() const; //to read body
//}}}
//{{{  access interface
public: //predicates
    virtual BOOL IsFoldP()      const;
    virtual BOOL IsTextP()      const;
    virtual BOOL IsEnterableP() const;
    virtual BOOL IsExitableP()  const;
    virtual int GetReadWriteIndent();
    virtual int Render();
    virtual void Render (CString &Buf) const;
    virtual CString GetChars (const int StartChar, const int EndChar);
    virtual CString GetChars (const int StartChar);
//}}}
//{{{  edit interface

public: //operations
    virtual void InsertChars (const int nChar, CString Chars);
    virtual void DeleteChar (const int nChar);
    virtual void DeleteChars (const int StartChar, const int EndChar);
    virtual void DeleteChars (const int StartChar);
    virtual void InsertChar (const int nChar, const UINT Char);

//}}}

};

//}}}

#endif
