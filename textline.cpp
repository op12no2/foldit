//{{{}}}

//{{{  header
// textline.cpp : implementation of the CTextLine class
//

//}}}
//{{{  includes
#include "stdafx.h"
#include "foldit.h"
#include "textline.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//}}}

//{{{  constructors
CTextLine::CTextLine (const char *Buf) : CLine (Buf)
{
}

CTextLine::CTextLine (const CTextLine &Source) : CLine (Source)
{
}
//}}}
//{{{  operators
CTextLine& CTextLine::operator=(const CTextLine &Source)
{
        *this = Source;
        return *this;
}
//}}}
//{{{  access interface
void CTextLine::Render (CString &Buf) const
{
        Buf = m_Body + "\r\n";
}
//}}}

