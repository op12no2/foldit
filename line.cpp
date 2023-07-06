//{{{}}}

//{{{  header
// line.cpp : implementation of the CLine class
//
//}}}

//{{{  includes
#include "stdafx.h"
#include "foldit.h"
#include "line.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//}}}

//{{{  constructors
CLine::CLine (const CLine &Source)
{
    *this = Source;
}
CLine::CLine (const char *Buf)
{
    if (Buf)
       m_Body = Buf;
    else
       m_Body = "";
}

//}}}
//{{{  destructor
CLine::~CLine()
{
    m_Body.Empty();
}
//}}}

//{{{  operators
CLine &CLine::operator=(const CLine& Source)
{
    m_Body = Source.m_Body;
    return *this;
}
CLine::operator const char * () const
{
    return (const char *)m_Body;
}
//}}}

//{{{  access interface
void CLine::Render (CString &Buf) const
{
    Buf = m_Body;
}

BOOL CLine::IsFoldP () const
{
    return FALSE;
}

BOOL CLine::IsTextP () const
{
    return FALSE;
}

BOOL CLine::IsEnterableP () const
{
    return FALSE;
}

BOOL CLine::IsExitableP () const
{
    return FALSE;
}
int CLine::GetReadWriteIndent()
{
    return 0;
}

int CLine::Render()
{
    return m_Body.GetLength();
}
CString CLine::GetChars (const int StartChar)
{
    return GetChars (StartChar, m_Body.GetLength() - 1);
}
CString CLine::GetChars (const int StartChar, const int EndChar)
{
    CString res;

    int Len = m_Body.GetLength();

    if (EndChar < StartChar)
       return res; //HACK

    res = m_Body.Mid (StartChar, EndChar - StartChar + 1);

    return res; //HACK
}
//}}}
//{{{  edit interface

void CLine::DeleteChar (const int nChar)
{
    int Len = m_Body.GetLength();

    m_Body = m_Body.Left (nChar) + m_Body.Right (Len - nChar - 1);

    return;
}

void CLine::DeleteChars (const int StartChar, const int EndChar)
{
    int Len = m_Body.GetLength();

    m_Body = m_Body.Left (StartChar) + m_Body.Right (Len - EndChar - 1);

    return;
}


void CLine::DeleteChars (const int StartChar)
{
    DeleteChars (StartChar, m_Body.GetLength() - 1);
    return;
}


void CLine::InsertChars (const int nChar, CString Chars)
{
    int Len = m_Body.GetLength();

    if (nChar >= Len)
      m_Body += Chars;
    else
      m_Body = m_Body.Left (nChar) + Chars + m_Body.Right (Len - nChar);
}

void CLine::InsertChar (const int nChar, const UINT Char)
{
    int Len = m_Body.GetLength();

    if (nChar >= Len)
      m_Body += CString((char)Char);
    else
      m_Body = m_Body.Left (nChar) + (char)Char + m_Body.Right (Len - nChar);

    return;
}

//}}}

