// Copyright 1994,5,6 Leyfield Systems Ltd
// foldline.cpp: CFoldLine class

//{{{  history
//
// 3/4/97   CRJ Added IsSubFoldP
// 3/4/97   CRJ Auto title new folds
// 3/4/97   CRJ Don't assume that line numbers from the view are valid
//

//}}}
//{{{  includes

#include "stdafx.h"
#include "foldit.h"
#include "textline.h"
#include "foldline.h"
#include "language.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}
//{{{  static

CString CFoldLine::m_Prefix ("...");

//}}}
//{{{  CFoldLine

CFoldLine::CFoldLine (const char *Buf) : CLine (Buf)
{
    m_Indent      = 0;
    m_pParent     = NULL;
    m_UsageCount  = 0;
}

//}}}
//{{{  CFoldLine

CFoldLine::CFoldLine (const CFoldLine &Source)
{
    CopyFoldFromTo (&Source, this);
}

//}}}
//{{{  Find

BOOL CFoldLine::Find (BOOL Limit, CString &What, int FromLine, int FromChar, CFoldLine **pFold, int &Line, int &Char)
{
    int nLine, nChar;
    CString search;

    nLine = FromLine;
    if (nLine == -1)
      nLine = 0;

    //may be null
    POSITION pos = m_LineList.FindIndex(nLine);

    while (pos)
    {
        CLine *pLine = (CLine *)m_LineList.GetNext (pos);

        search = pLine->m_Body;
        search.MakeLower();
        if (FromChar)
        {
          if (pLine->IsFoldP())
            FromChar -= ((CFoldLine *)pLine)->GetReadWriteIndent();
          search = search.Right(search.GetLength()-FromChar);
        }
        if ((nChar = search.Find (What)) >= 0)
        {
             nChar += FromChar;
             if (pLine->IsFoldP())
                 nChar += ((CFoldLine *)pLine)->GetReadWriteIndent();

             *pFold = this;
             Line = nLine;
             Char = nChar;
             return TRUE;
        }

        if (pLine->IsFoldP())
        {
            if (((CFoldLine *)pLine)->Find (Limit, What, -1, 0, pFold, Line, Char))
                   return TRUE;
        }
        FromChar = 0; //start next line at 0
        nLine++;
    }

    if (FromLine != -1 && this != App()->m_pLastFind)
    {
      int i = 0;
      pos = m_pParent->m_LineList.GetHeadPosition();
      while (pos)
      {
        if ((CLine *)m_pParent->m_LineList.GetNext(pos) == this)
          return m_pParent->Find (Limit, What, i+1, 0, pFold, Line, Char);
        i++;
      }
      ASSERT (TRUE);
    }

    return (FALSE);
}

//}}}
//{{{  operator=

CFoldLine &CFoldLine::operator=(const CFoldLine &Source)
{
    CopyFoldFromTo (&Source, this);
    return *this;
}

//}}}
//{{{  access interface

//{{{  IsEnterableAt

BOOL CFoldLine::IsEnterableAtP (const int nLine) const
{
    ASSERT (nLine >= 0);

    CLine *pLine = GetLineAt (nLine);
    if (pLine != NULL)
      return pLine->IsEnterableP();
    else
      return FALSE;
}
//}}}
//{{{  IsActiveKidsP

BOOL CFoldLine::IsActiveKidsP() const
{
    BOOL res = FALSE;

    POSITION pos = m_LineList.GetHeadPosition();

    while (pos)
    {
        CLine *pLine = (CLine *)m_LineList.GetNext(pos);

        if (pLine->IsFoldP())
        {
            res = ((CFoldLine *)pLine)->IsActiveP();

            if (res)
                return TRUE;

            res = ((CFoldLine *)pLine)->IsActiveKidsP();

            if (res)
                return TRUE;
        }
    }

    return FALSE;
}
//}}}
//{{{  IsSubFoldP

BOOL CFoldLine::IsSubFoldP(CLine *pThis) const
{
    if (((CLine *)this) == pThis) return TRUE;

    POSITION pos = m_LineList.GetHeadPosition();
    while (pos)
    {
      CLine *pLine = (CLine *)m_LineList.GetNext(pos);
      if (pLine->IsFoldP())
      {
        if (pLine == pThis)
          return TRUE;
        else
        {
          if (((CFoldLine *)pLine)->IsSubFoldP(pThis))
            return TRUE;
        }
      }
    }
    return FALSE;
}
//}}}
//{{{  IsActiveAtP

BOOL CFoldLine::IsActiveAtP (int StartLine, int EndLine)
{
    int nLines = EndLine - StartLine + 1;
    ASSERT (nLines > 0);

    POSITION pos = m_LineList.FindIndex (StartLine);
    ASSERT (pos);

    while (nLines--)
    {
        CLine *pLine = (CLine *)m_LineList.GetNext(pos);
        ASSERT (pLine);

        if (pLine->IsFoldP())
            if (((CFoldLine *)pLine)->IsActiveKidsP() || ((CFoldLine *)pLine)->IsActiveP())
                return TRUE;
    }

    return FALSE;
}
//}}}
//{{{  CopyFoldFromTo

void CFoldLine::CopyFoldFromTo (const CFoldLine *pFrom, CFoldLine *pTo)
{
    ASSERT (pFrom->IsFoldP());
    ASSERT (pTo->IsFoldP());

    //same title as source fold
    pTo->m_Body         = pFrom->m_Body;

    //indent is meaningless for top level - sub fold get done later
    pTo->m_Indent       = pFrom->m_Indent;

    //top level parent is meangingless - sub-folds later
    pTo->m_pParent      = NULL;

    //copying so this fold is not be used
    pTo->m_UsageCount   = 0;

    POSITION pos = pFrom->m_LineList.GetHeadPosition();

    while (pos)
    {
        CLine *pLine = (CLine *)(pFrom->m_LineList.GetNext(pos));
        ASSERT (pLine);

        if (pLine->IsFoldP())
        {
            //replicate the source fold in the destination, note that pParent will
            //be different (copying) and the usage count is not valid
            CFoldLine *pNewFold = new CFoldLine;
            ASSERT (pNewFold);
            MEM (pNewFold);

            //recursively do the copy
            CopyFoldFromTo ((CFoldLine *)pLine, pNewFold);

            //add the new sub-fold to the dest fold list
            pTo->m_LineList.AddTail(pNewFold);

            //and characterise it
            pNewFold->m_pParent = pTo;
            pNewFold->m_Indent  = ((CFoldLine *)pLine)->m_Indent;

            continue;
        }

        if (pLine->IsTextP())
        {
            CTextLine *pNewText = new CTextLine (*((CTextLine *)pLine));
            ASSERT (pNewText);
            MEM (pNewText);

            pTo->m_LineList.AddTail(pNewText);

            continue;
        }
    }
}
//}}}
//{{{  GetChars

CString CFoldLine::GetChars (const int StartChar)
{
    return GetChars (StartChar, Render()-1);
}
//}}}
//{{{  GetChars

CString CFoldLine::GetChars (const int StartChar, const int EndChar)
{
    ASSERT (CanDeleteCharsP(StartChar, EndChar));

    if (StartChar < m_Indent)
        //all spaces
        return CString (' ', EndChar - StartChar);

    //in title
    return CLine::GetChars (StartChar - GetReadWriteIndent(), EndChar - GetReadWriteIndent());
}
//}}}
//{{{  GetLineAt

CLine *CFoldLine::GetLineAt (const int nLine) const
{
    ASSERT (nLine >= 0);

    POSITION pos = m_LineList.FindIndex(nLine);
    if (pos != NULL)
      return (CLine *)m_LineList.GetAt(pos);
    else
      return NULL;
}
//}}}
//{{{  Render

int CFoldLine::Render()
{
    return GetReadWriteIndent() + GetTitleLength();
}
//}}}
//{{{  Render

void CFoldLine::Render (CString &Buf) const
{
    int Len = m_Indent + GetPrefix().GetLength() + m_Body.GetLength() + 4;

    Buf.GetBufferSetLength (Len);

    Buf = CString (' ', m_Indent);
    Buf += GetPrefix();
    Buf += "  ";
    Buf += m_Body;
    Buf += "\r\n";

    Buf.ReleaseBuffer();
}
//}}}
//{{{  GetTitleLength

int CFoldLine::GetTitleLength()
{
    return m_Body.GetLength();
}

//}}}
//{{{  GetReadWriteIndent

int CFoldLine::GetReadWriteIndent()
{
    return  m_Indent + GetPrefix().GetLength() + 2;
}
//}}}

//}}}
//{{{  edit interface

//{{{  CanDeleteCharAtP

BOOL CFoldLine::CanDeleteCharAtP (const int nLine, const int nChar)
{
    ASSERT (nLine >= 0);
    ASSERT (nChar >= 0);

    POSITION pos = m_LineList.FindIndex (nLine);
    if (pos == NULL)
      return FALSE;

    CLine *pLine = (CLine *)m_LineList.GetNext(pos);
    ASSERT (pLine);

    if (pLine->IsFoldP())
        if (pLine->Render() == nChar)
            //can't delete at EOFL
            return FALSE;
        else
            return ((CFoldLine *)pLine)->CanDeleteCharP(nChar);

    //a text line

    //check for position at end of line (text line itself can't handle this
    //because a line only has a self context, it does not know about
    //its peers)
    if (nChar >= pLine->Render())
    {
        if (!pos)
            //last line - nothing to join with
            return FALSE;

        //deleting at end of line - must join with next line

        if (pLine->m_Body.IsEmpty())
            //empty line
            return TRUE;

        CLine *pNextLine = (CLine *)m_LineList.GetAt(pos);
        ASSERT (pNextLine);

        if (pNextLine->IsFoldP())
            //next line is fold - cant join to fold
            return FALSE;

        return TRUE;
    }

    //deleting char within line (not at end)
    return TRUE;
}
//}}}
//{{{  DeleteCharAt

void CFoldLine::DeleteCharAt (const int nLine, const int nChar)
{
    POSITION pos = m_LineList.FindIndex (nLine);
    ASSERT (pos);

    CLine *pLine = (CLine *)m_LineList.GetNext(pos);
    ASSERT (pLine);

    //if a fold then perform delete using foldline itself
    if (pLine->IsFoldP())
    {
        pLine->DeleteChar(nChar);
        return;
    }

    //a text line

    //check for position at end of line (text line itself can't handle this
    //because a line only has a self context, it does not know about
    //its peers)
    if (nChar >= pLine->Render())
    {
        //deleting at end of line - must join with next line
        if (pLine->m_Body.IsEmpty())
        {
            //special case of empty line - just delete it
            DeleteLineAt (nLine);
            return;
        }

        CLine *pNextLine = (CLine *)m_LineList.GetAt(pos);
        ASSERT (pNextLine);
        ASSERT (!pNextLine->IsFoldP());

        //concatenate line bodies
        pLine->m_Body += pNextLine->m_Body;

        //and remove now redundant line
        m_LineList.RemoveAt(pos);
        delete pNextLine;

        return;
    }

    //deleting char within line (not at end)
    pLine->DeleteChar (nChar);
}
//}}}

//{{{  CanDeleteCharP

BOOL CFoldLine::CanDeleteCharP (const int nChar)
{
    ASSERT (nChar >= 0);
    ASSERT (nChar < Render());

    return (nChar < m_Indent) ||
           ((nChar >= GetReadWriteIndent()) && (nChar < Render()));
}
//}}}
//{{{  DeleteChar

void CFoldLine::DeleteChar (const int nChar)
{
    ASSERT (nChar >= 0);
    ASSERT (nChar < Render());

    int nAt = nChar;
    int PrefixLen = GetPrefix().GetLength() + 2;

    if (nAt < m_Indent)
    {
        m_Indent--;
        return;
    }

    nAt -= m_Indent;
    ASSERT (nAt >= PrefixLen);

    nAt-= PrefixLen;

    CLine::DeleteChar (nAt);
}
//}}}

//{{{  CanDeleteCharsP

BOOL CFoldLine::CanDeleteCharsP (const int StartChar, const int EndChar)
{
    ASSERT (StartChar >= 0);
    ASSERT (EndChar   >= 0);
    ASSERT (StartChar <= EndChar);
    ASSERT (EndChar   < Render());

    if ((StartChar < m_Indent) && (EndChar < m_Indent))
        return TRUE;

    if ((StartChar >= GetReadWriteIndent()) && (EndChar >= GetReadWriteIndent()))
        return TRUE;

    return FALSE;
}
//}}}
//{{{  DeleteChars

void CFoldLine::DeleteChars (const int StartChar, const int EndChar)
{
    ASSERT (StartChar >= 0);
    ASSERT (EndChar   >= 0);
    ASSERT (StartChar <= EndChar);
    ASSERT (EndChar   < Render());

    for (int i = StartChar; i <= EndChar; i++)
        DeleteChar(StartChar);
}
//}}}

//{{{  CanDeleteCharsP

BOOL CFoldLine::CanDeleteCharsP (const int StartChar)
{
    return CanDeleteCharsP (StartChar, Render()-1);
}
//}}}
//{{{  DeleteChars

void CFoldLine::DeleteChars (const int StartChar)
{
    DeleteChars (StartChar, Render()-1);
}
//}}}

//{{{  CanInsertCharAtP

POSITION CFoldLine::CanInsertCharAtP (const int nLine, const int nChar, const UINT Char)
{
    ASSERT (nLine >= 0);
    ASSERT (nChar >= 0);

    POSITION pos = m_LineList.FindIndex(nLine);
    if (pos == NULL)
      return pos;

    CLine *pLine = (CLine *)m_LineList.GetAt (pos);
    ASSERT (pLine);

    if (pLine->IsFoldP())
        if (((CFoldLine *)pLine)->CanInsertCharP (nChar, Char))
                return pos;
         else
             return NULL;
    else
        return pos;
}
//}}}
//{{{  InsertCharAt

void CFoldLine::InsertCharAt (POSITION pos, const int nChar, const UINT Char)
{
    if (pos == NULL) return;
    CLine *pLine = (CLine *)m_LineList.GetAt (pos);
    ASSERT (pLine);

    pLine->InsertChar (nChar, Char);
}
//}}}

//{{{  CanInsertCharP

BOOL CFoldLine::CanInsertCharP (const int nChar, const UINT Char)
{
   ASSERT (nChar >= 0);
   ASSERT (nChar <= Render());

   return ((nChar <= m_Indent) && (Char == VK_SPACE)) ||
           (nChar >= GetReadWriteIndent());
}
//}}}
//{{{  InsertChar

void CFoldLine::InsertChar (const int nChar, const UINT Char)
{
    ASSERT (nChar >= 0);
    ASSERT (nChar <= Render());

    int nAt = nChar;
    int PrefixLen = GetPrefix().GetLength() + 2;

    if (nAt <= m_Indent)
    {
        m_Indent++;
        return;
    }

    nAt -= m_Indent;
    nAt -= PrefixLen;
    ASSERT (nAt >= 0);

    CLine::InsertChar (nAt, Char);
}

//}}}
//{{{  InsertChars

void CFoldLine::InsertChars (const int nChar, CString Chars)
{
    ASSERT (nChar >= 0);

    int nAt = nChar;
    int PrefixLen = GetPrefix().GetLength() + 2;

    nAt -= m_Indent;
    nAt -= PrefixLen;
    if (nAt < 0) nAt = 0;

    CLine::InsertChars (nAt, Chars);
}

//}}}

//{{{  CanCreateChildFoldAt

BOOL CFoldLine::CanCreateChildFoldAtP (const int nStart, const int nLen, const int nIndent)
{
    ASSERT (nStart  >= 0);
    ASSERT (nLen    >= 0);
    ASSERT (nIndent >= 0);

    int last = nStart+nLen;
    if (nLen) last -= 1;

    POSITION pos = m_LineList.FindIndex(last);
    if (pos == NULL) return FALSE;

    return TRUE;
}
//}}}
//{{{  CreateChildFoldAt

void CFoldLine::CreateChildFoldAt (const int nStart, const int nLen, const int nIndent)
{
    if (!CanCreateChildFoldAtP(nStart,nLen,nIndent)) return;

    BOOL DoneTitle = FALSE;

    ASSERT (nStart  >= 0);
    ASSERT (nLen    >= 0);
    ASSERT (nIndent >= 0);

    CFoldLine *pNewFold = new CFoldLine;
    MEM(pNewFold);

    pNewFold->m_Indent  = nIndent;     //relative to parent
    pNewFold->m_pParent = this;        //is a child of current fold
    pNewFold->m_Body    = "";

    POSITION StartPos = m_LineList.FindIndex (nStart);
    ASSERT (StartPos);

    m_LineList.InsertBefore (StartPos, pNewFold);

    if (!nLen)
    {
        pNewFold->m_LineList.AddTail (new CTextLine);
        return;
    }

    //move lines to new fold
    for (int nNext=0; nNext < nLen; nNext++)
    {
        //get pointer to next line in selection
        CLine *pNextLine = (CLine *)m_LineList.GetAt(StartPos);
        ASSERT (pNextLine);

        if ((pNextLine->m_Body.GetLength() > 2) && !DoneTitle)
        {
          pNewFold->m_Body = pNextLine->m_Body;
          DoneTitle = TRUE;
        }

        //remove the indent from the next line
        if (pNextLine->IsFoldP())
        {
           ((CFoldLine *)pNextLine)->m_Indent -= nIndent;
           if (((CFoldLine *)pNextLine)->m_Indent < 0)
             ((CFoldLine *)pNextLine)->m_Indent = 0;
        }
        else
        {
          int LineLen = pNextLine->m_Body.GetLength() - nIndent;
          if (LineLen < 0)
            LineLen = 0;

          pNextLine->m_Body = pNextLine->m_Body.Right(LineLen);
        }

        //add to new fold
        pNewFold->m_LineList.AddTail (pNextLine);

        //if any of these lines are folds, replace their parent to
        //be the new fold.
        if (pNextLine->IsFoldP())
            ((CFoldLine *)pNextLine)->m_pParent = pNewFold;

        //remove from existing fold
        POSITION ThisPos = StartPos;
        m_LineList.GetNext(StartPos);
        m_LineList.RemoveAt (ThisPos);
    }
}
//}}}

//{{{  CanCreateTailFoldP

BOOL CFoldLine::CanCreateTailFoldAtP (const int nIndent)
{
        ASSERT (nIndent >= 0);
        return TRUE;
}
//}}}
//{{{  CreateTailFoldAt

void CFoldLine::CreateTailFoldAt (const int nIndent)
{
    CFoldLine *pNewFold = new CFoldLine;
    ASSERT (pNewFold);
    MEM (pNewFold);

    pNewFold->m_Indent  = nIndent;     //relative to parent
    pNewFold->m_pParent = this;        //is a child of current fold
    pNewFold->m_Body    = "<title>";

    m_LineList.AddTail (pNewFold);

    pNewFold->m_LineList.AddTail (new CTextLine);
}
//}}}

//{{{  CanCutCopyDelAtP

BOOL CFoldLine::CanCutCopyDelAtP (int StartLine, int StartChar, int EndLine, int EndChar,
                                  BOOL DoCut, BOOL DoCopy)
{
    int nLines = EndLine - StartLine + 1;
    ASSERT (nLines > 0);

    ASSERT (EndChar   >= 0);
    ASSERT (StartChar >= 0);

    POSITION startpos = m_LineList.FindIndex (StartLine);
    POSITION endpos   = m_LineList.FindIndex (EndLine);
    if (startpos == NULL) return FALSE;
    if (endpos   == NULL) return FALSE;

    if (DoCut)
        if (IsActiveAtP (StartLine, EndLine))
                return FALSE;

    CLine *pStartLine = (CLine *)m_LineList.GetNext(startpos);
    ASSERT (pStartLine);

    CLine *pEndLine = (CLine *)m_LineList.GetNext(endpos);
    ASSERT (pEndLine);

    if (pStartLine->IsFoldP())
    {
        if (nLines == 1)
        {
            //OK if whole line sel
            if ((StartChar <= ((CFoldLine *)pStartLine)->m_Indent) &&
                (EndChar   == (pStartLine->Render()-1)))
                return TRUE;

            //OK if title sel
            if (StartChar >= pStartLine->GetReadWriteIndent())
            {
                ASSERT (EndChar < pStartLine->Render());
                return TRUE;
            }

            //OK if indent sel
            if (EndChar < ((CFoldLine *)pStartLine)->m_Indent)
                return TRUE;

            //else none of the above and 1 line sel so fail
            return FALSE;
        }

        //not OK if lines sel > 1 and not whole fold sel
        //ie from SOL or prefix start
        if (StartChar >= ((CFoldLine *)pStartLine)->m_Indent)
            if (StartChar != 0)
                return FALSE;
    }

    //first line is text or whole sel fold and nLines > 1

    if (pEndLine->IsFoldP())
    {
        if (EndChar == (pEndLine->Render() - 1))
            //last line if fold but sel to EOL so OK
            return TRUE;

        //if end line is fold whole fold must be selected
        return FALSE;
    }

    //start/end lines are text lines
    return TRUE;
}
//}}}
//{{{  CutCopyDelAt

void CFoldLine::CutCopyDelAt (int StartLine, int StartChar, int EndLine, int EndChar,
                              BOOL DoCut, BOOL DoCopy, CLanguage *pLang, BOOL CopyToClipboard)
{
    int nLines = EndLine - StartLine + 1;
    ASSERT (nLines > 0);

    CFoldLine *pNewFold;
    CTextLine *pNewLine;

    //handle first line
    POSITION pos    = m_LineList.FindIndex(StartLine);
    POSITION oldpos = pos;
    ASSERT (oldpos);

    CLine *pLine = (CLine *)m_LineList.GetNext(pos);
    ASSERT (pLine);

    CFoldLine *pPaste = NULL;

    if (DoCopy)
        pPaste = App()->NewPaste("");

    ASSERT(pPaste);

    if (pLine->IsFoldP())
    {
        if (EndChar < ((CFoldLine *)pLine)->m_Indent)
        {
            //single line sel in indent area of fold
            if (DoCopy)
            {
                pNewLine = new CTextLine (pLine->GetChars(StartChar, EndChar));
                ASSERT (pNewLine);
                MEM (pNewLine);
                pPaste->m_LineList.AddTail (pNewLine);
            }
            if (DoCut)
            {
                pLine->DeleteChars(StartChar, EndChar);
            }

            if (DoCopy && CopyToClipboard)
                App()->CopyToClipboard(pLang);
            return;
        }

        if (StartChar <= ((CFoldLine *)pLine)->m_Indent)
        {
            //whole fold is selected
            if (DoCopy)
            {
                pNewFold = new CFoldLine(*((CFoldLine *)pLine));
                ASSERT(pNewFold);
                MEM(pNewFold);
                pPaste->m_LineList.AddTail (pNewFold);
                pNewFold->m_pParent = pPaste;
                pNewFold->m_Indent = ((CFoldLine *)pLine)->m_Indent;
            }
            if (DoCut)
            {
                delete pLine;
                m_LineList.RemoveAt(oldpos);
            }
        }
        else
        {
            //valid title text selected and single line sel
            if (DoCopy)
            {
                pNewLine = new CTextLine (pLine->GetChars(StartChar, EndChar));
                ASSERT (pNewLine);
                MEM (pNewLine);
                pPaste->m_LineList.AddTail (pNewLine);
            }
            if (DoCut)
            {
                pLine->DeleteChars(StartChar, EndChar);
            }

            if (DoCopy && CopyToClipboard)
                App()->CopyToClipboard(pLang);
            return;
        }
    }
    else
    {
        //first line is text - handle partial/whole line sel
        if (nLines == 1)
        {
            //end of sel may be before EOL
            if (DoCopy)
            {
                pNewLine = new CTextLine (pLine->GetChars (StartChar, EndChar));
                ASSERT (pNewLine);
                MEM (pNewLine);
                pPaste->m_LineList.AddTail (pNewLine);
            }
            if (DoCut && pLine->Render())
                pLine->DeleteChars (StartChar, EndChar);
        }
        else
        {
            //delete to EOL
            if (DoCopy)
            {
                pNewLine = new CTextLine (pLine->GetChars (StartChar));
                ASSERT (pNewLine);
                MEM (pNewLine);
                pPaste->m_LineList.AddTail (pNewLine);
            }
            if (DoCut && pLine->Render())
                pLine->DeleteChars (StartChar);
        }

        //delete line if no text left
        if (!pLine->Render()  && DoCut)
        {
            delete pLine;
            m_LineList.RemoveAt (oldpos);
        }
    }

    if (nLines == 1)
    {
        if (DoCopy && CopyToClipboard)
            App()->CopyToClipboard(pLang);
        return;
    }

    //at least two lines if we get to here
    //now delete all full lines; ie all but last lines; may be null while loop
    int nFullLines = nLines;

    while (--nFullLines > 1)
    {
        oldpos = pos;
        pLine = (CLine *)m_LineList.GetNext(pos);
        ASSERT (pLine);

        if (pLine->IsFoldP())
        {
            if (DoCopy)
            {
                pNewFold = new CFoldLine (*((CFoldLine *)pLine));
                ASSERT (pNewFold);
                MEM (pNewFold);
                pNewFold->m_pParent = pPaste;
                pNewFold->m_Indent = ((CFoldLine *)pLine)->m_Indent;

                pPaste->m_LineList.AddTail(pNewFold);
            }
        }

        else
        {
            if (DoCopy)
            {
                pNewLine = new CTextLine (*(CTextLine *)pLine);
                ASSERT (pNewLine);
                MEM (pNewLine);
                pPaste->m_LineList.AddTail(pNewLine);
            }
        }

        if (DoCut)
        {
            m_LineList.RemoveAt(oldpos);
            delete pLine;
        }
    }

    //there will be a last line if we get to here
    //now handle last line (as pointed to by pos)

    pLine = (CLine *)m_LineList.GetAt(pos);

    if (pLine->IsFoldP())
    {
        //whole line is selected
        if (DoCopy)
        {
            pNewFold = new CFoldLine (*((CFoldLine *)pLine));
            ASSERT (pNewFold);
            MEM (pNewFold);
            pPaste->m_LineList.AddTail (pNewFold);
            pNewFold->m_pParent = pPaste;
            pNewFold->m_Indent = ((CFoldLine *)pLine)->m_Indent;
        }
        if (DoCut)
        {
            m_LineList.RemoveAt(pos);
            delete pLine;
        }
    }

    else
    {
        //left side of line to EndChar is selected
        if (DoCopy)
        {
            pNewLine = new CTextLine (pLine->GetChars(0, EndChar));
            ASSERT (pNewLine);
            MEM (pNewLine);
            pPaste->m_LineList.AddTail(pNewLine);
        }

        if (DoCut)
        {
            if (pLine->Render())
                pLine->DeleteChars(0, EndChar);

            if (!pLine->Render())
            {
                delete pLine;
                m_LineList.RemoveAt (pos);
            }
        }
    }

    if (DoCopy && CopyToClipboard)
        App()->CopyToClipboard(pLang);
    return;
}
//}}}

//{{{  CanPasteAtP

//assumes that the sel is not highlighted

BOOL CFoldLine::CanPasteAtP (int Line, int Char)
{
    ASSERT (Line >= 0);
    ASSERT (Char >= 0);

    POSITION pos = m_LineList.FindIndex (Line);
    if (pos == NULL) return FALSE;

    return TRUE;
}
//}}}
//{{{  PasteAt

void CFoldLine::PasteAt (int Line, int Char)
{
    CFoldLine *pPaste = App()->m_pPasteFold;

    //see if it's a single text line - if so insert it
    if (pPaste->m_LineList.GetCount() == 1)
    {
      POSITION pos = pPaste->m_LineList.FindIndex(0);
      CLine *pFromLine = (CLine *)(pPaste->m_LineList.GetAt(pos));
      if (!pFromLine->IsFoldP())
      {
         //pasting one line of text so insert it
         pos = m_LineList.FindIndex(Line);
         CLine *pToLine = (CLine *)(m_LineList.GetAt(pos));
         pToLine->InsertChars(Char,pFromLine->m_Body); //will invoke CFoldLine::InsertChars()
         return;
      }
    }

    //not a single text line

    CFoldLine *pNew = new CFoldLine (*pPaste);
    MEM(pNew);
    BOOL Split = FALSE;

    if (CanSplitLineAtP(Line, Char))
    {
        Split = TRUE;
        SplitLineAt (Line, Char);
    }

    POSITION pos = m_LineList.FindIndex(Line);
    ASSERT (pos);

    pNew->m_pParent= this;
    pNew->m_Indent = 0;
    m_LineList.InsertAfter (pos, pNew);

    ASSERT (CanUnFoldAtP(Line + 1));
    UnFoldAt (Line + 1);

    if (CanDeleteCharAtP (Line, Char) && Split)
        DeleteCharAt (Line, Char);

    return;
}
//}}}

//{{{  CanDeleteLineAtP

BOOL CFoldLine::CanDeleteLineAtP (const int nLine)
{
    ASSERT (nLine >= 0);

    POSITION pos = m_LineList.FindIndex (nLine);
    if (pos == NULL) return FALSE;

    if (!m_LineList.GetCount())
        return FALSE;

    return TRUE;
}

//}}}
//{{{  DeleteLineAt

void CFoldLine::DeleteLineAt (const int nLine)
{
    POSITION pos = m_LineList.FindIndex (nLine);
    ASSERT (pos);

    CLine *pLine = (CLine *)m_LineList.GetAt (pos);
    ASSERT (pLine);

    delete pLine;
    m_LineList.RemoveAt (pos);

    return;
}
//}}}

//{{{  CanUnFoldAtP

BOOL CFoldLine::CanUnFoldAtP (const int nLine)
{
    ASSERT (nLine >= 0);

    //get position of child fold to unfold
    POSITION pos = m_LineList.FindIndex (nLine);
    if (pos == NULL) return FALSE;

    //get pointer to it
    CFoldLine *pFold = (CFoldLine *)m_LineList.GetAt(pos);
    ASSERT (pFold);

    if (!pFold->IsFoldP())
        return FALSE;

    if (pFold->IsActiveP())
        return FALSE;

    return TRUE;
}
//}}}
//{{{  UnFoldAt

void CFoldLine::UnFoldAt (const int nLine)
{
    //get position of child fold to unfold
    POSITION AtPos = m_LineList.FindIndex(nLine);
    ASSERT (AtPos);

    //get pointer to it
    CFoldLine *pFold = (CFoldLine *)m_LineList.GetAt(AtPos);
    ASSERT (pFold);
    ASSERT (pFold->IsFoldP());

    //initiate iteration of child fold
    POSITION pos = pFold->m_LineList.GetHeadPosition();

    CString Indent (' ',pFold->m_Indent);

    //iterate the child fold copying items to the parent
    while (pos != NULL)
    {
        //get pointer to next child fold line
        CLine *pNext = (CLine *)pFold->m_LineList.GetNext(pos);
        ASSERT (pNext);

        //and copy it to the parent
        m_LineList.InsertBefore (AtPos, pNext);

        //adjust parent pointer for child fold fold lines
        //also adjust indent
        if (pNext->IsFoldP())
        {
            ((CFoldLine *)pNext)->m_pParent = this;
            ((CFoldLine *)pNext)->m_Indent += pFold->m_Indent;
        }

        //adjust text to take account of indent for child fold text lines
        if (pNext->IsTextP())
        {
            pNext->m_Body = Indent + pNext->m_Body;
        }
    }

    //delete the unfolded fold
    pFold->m_LineList.RemoveAll();
    delete pFold;

    //and remove from parent list
    m_LineList.RemoveAt (AtPos);

    return;
}
//}}}

//{{{  CanSplitLineAtP

BOOL CFoldLine::CanSplitLineAtP (const int nLine, const int nChar)
{
    ASSERT (nLine >= 0);
    ASSERT (nChar >= 0);

    POSITION pos = m_LineList.FindIndex (nLine);
    if (pos == NULL) return FALSE;

    CLine *pLine = (CLine *)m_LineList.GetAt (pos);
    ASSERT (pLine);

    if (pLine->IsFoldP())
    {
        CFoldLine *pFold = (CFoldLine *)pLine;

        //only allow line split before prefix
        //or at end of line

        if (nChar <= pFold->m_Indent)
            return TRUE;

        if (nChar >= pFold->Render())
            return TRUE;

        return FALSE;
    }

    return TRUE;
}
//}}}
//{{{  SplitLineAt

void CFoldLine::SplitLineAt (const int nLine, const int nChar)
{
    POSITION pos = m_LineList.FindIndex (nLine);
    ASSERT (pos);

    CLine *pLine = (CLine *)m_LineList.GetAt (pos);
    ASSERT (pLine);

    if (pLine->IsFoldP())
    {
        CFoldLine *pFold = (CFoldLine *)pLine;

        if (nChar <= pFold->m_Indent)
        {
            CTextLine *pNew = new CTextLine;
            ASSERT (pNew);
            MEM (pNew);

            m_LineList.InsertBefore (pos, pNew);

            return;
        }

        if (nChar >= pFold->Render())
        {
            CTextLine *pNew = new CTextLine;
            MEM(pNew);
            m_LineList.InsertAfter (pos, pNew);

            return;
        }

        ASSERT (TRUE);
    }

    CTextLine *pNew = new CTextLine;
    MEM(pNew);

    //splitting an existing text line
    //insert new line after present line
    m_LineList.InsertAfter (pos, pNew);

    pNew->m_Body = pLine->m_Body.Right (pLine->m_Body.GetLength() - nChar);
    pLine->m_Body = pLine->m_Body.Left (nChar);

    return;
}
//}}}

//{{{  WriteFoldsToString

void CFoldLine::WriteFoldsToString (CFoldLine *pFold, CString &UnfoldedStr, CLanguage *pLang)
{
    CFoldLine *pCurrent = pFold;

    UnfoldedStr = "";

    ASSERT (pLang);
    WriteFold (pCurrent, 0, UnfoldedStr, pLang);
}
//}}}
//{{{  WriteFold

void CFoldLine::WriteFold (CFoldLine *pFold, int CurIndent, CString &UnfoldedStr, CLanguage *pLang)
{
    CString CurIndentStr (' ', CurIndent);
    //CString AddIndentStr (' ', pFold->m_Indent);

    CSentence Str;

    Str = CurIndentStr + pLang->m_SCom + pLang->m_SFold + "  ";
    Str += pFold->m_Body + pLang->m_ECom + "\r\n";

    if (pFold->m_pParent != &(App()->m_PasteStack))
        UnfoldedStr += Str;

    POSITION pos = pFold->m_LineList.GetHeadPosition();

    while (pos)
    {
        CLine *pLine = (CLine *)pFold->m_LineList.GetNext (pos);

        if (pLine->IsFoldP())
            WriteFold ((CFoldLine *)pLine, CurIndent + ((CFoldLine *)pLine)->m_Indent, UnfoldedStr, pLang);

        else
            //WriteString (CurIndentStr + AddIndentStr + pLine->m_Body + "\n");
            UnfoldedStr += CurIndentStr + pLine->m_Body + "\r\n";
    }

    Str = CurIndentStr + pLang->m_SCom + pLang->m_EFold + pLang->m_ECom + "\r\n";

    if (pFold->m_pParent != &(App()->m_PasteStack))
      UnfoldedStr += Str;
}
//}}}

//{{{  RemoveTrailing
//
// remove trailing blank lines.  A line with just spaces is
// considered to be a blank line.  If there is more than one blank
// trailing blank line then leave just one.
//

void CFoldLine::RemoveTrailing()
{
    CSentence body;
    POSITION pos;
    while((pos=m_LineList.GetTailPosition()) && m_LineList.GetCount() > 1)
    {
      CLine *pLine = (CLine *)m_LineList.GetPrev(pos);
      if (pLine->IsFoldP())
        break;                        //hit a fold
      body = pLine->m_Body;
      body.RemoveTrailingWhite();
      if (body.GetLength())
        break;                        //hit a non-blank line

      pLine = (CLine *)m_LineList.GetAt(pos);
      if (pLine->IsFoldP())
        break;                        //hit a fold
      body = pLine->m_Body;
      body.RemoveTrailingWhite();
      if (body.GetLength())
        break;                        //hit a non-blank line

      delete pLine;
      m_LineList.RemoveAt(pos);
    }
}
//}}}

//{{{  CanOutdentP

BOOL CFoldLine::CanOutdentP(const int StartLine, const int EndLine)
{
    int i;
    POSITION pos;
    CLine *pLine;
    CFoldLine *pFold;

    for (i=StartLine; i<=EndLine; i++)
    {
      pos = m_LineList.FindIndex(i);
      if (pos == NULL) continue;
      pLine = (CLine *)m_LineList.GetAt(pos);
      if (pLine->IsFoldP())
      {
        pFold = (CFoldLine *)pLine;
        if (!pFold->m_Indent)
          return FALSE;
      }
      else
      {
        if ((pLine->m_Body.GetLength()) && pLine->m_Body[0] != ' ')
          return FALSE;
      }
    }

    return TRUE;
}
//}}}
//{{{  Outdent

void CFoldLine::Outdent(const int StartLine, const int EndLine)
{
    int i;
    POSITION pos;
    CLine *pLine;
    CFoldLine *pFold;

    for (i=StartLine; i<=EndLine; i++)
    {
      pos = m_LineList.FindIndex(i);
      if (pos == NULL) continue;
      pLine = (CLine *)m_LineList.GetAt(pos);
      if (pLine->IsFoldP())
      {
        pFold = (CFoldLine *)pLine;
        pFold->DeleteChar(0);
      }
      else
      {
        if (pLine->m_Body.GetLength())
          pLine->DeleteChar(0);
      }
    }
}
//}}}

//}}}

