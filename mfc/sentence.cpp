//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd
// sentence.cpp: CSentence class

//{{{  history
//
// 07/03/05 CRJ Add second version of ExtractBEtween
//

//}}}
//{{{  includes

#include "stdafx.h"
#include "foldit.h"
#include "sentence.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


//}}}
//{{{  CSentence

CSentence::~CSentence ()
{
}

//}}}
//{{{  FindTrailingWhite

int CSentence::FindTrailingWhite ()
{
    int i;

    if (IsEmpty())
        return 0;

    for (i=GetLength()-1; i>=0; i--)
      if (GetAt(i) != ' ')
        return i+1;

    return 0;
}

//}}}
//{{{  StripLine

int CSentence::StripLine (char *Buf, int *Next)
{
    CString Part;
    int i;

    if (*Next >= GetLength()) return FALSE;
    if (GetLength() == 0)     return FALSE;

    Part = Mid(*Next);
    TRACE1("NextPart=%s\n",(const char *)Part);
    int EndLine = Part.Find('\r');

    if (EndLine == -1) EndLine = Part.GetLength();

    for (i=*Next; i<*Next+EndLine; i++)
      Buf[i-*Next] = GetAt(i);

    *Next+=EndLine+2;

    Buf[EndLine]='\n';
    Buf[EndLine+1]='\0';

    TRACE1 ("StripLine=|%s|",Buf);
    TRACE1 ("Next=%d\n",*Next);
    return TRUE;
}

//}}}
//{{{  ReadFoldsTo

void CSentence::ReadFoldsTo (CFoldLine *pFold, CLanguage *pDocLang)
{
    //{{{  context
    
    int StartAt;
    CFoldLine *pNewFold;
    CSentence Buf;
    char RawLine[U_FILEBUFFER];
    CLanguage *pLang;
    
    CFoldLine *pCurrent = pFold;
    
    int FullIndent = 0;
    int RawLineLen;
    int Next=0;
    
    CString StartFoldMark, EndFoldMark, LangSpecMark, LazyEndFoldMark;
    
    //}}}
    //{{{  init
    
    pLang= App()->m_pLanguages->FindLanguage(*this,"");
    if (pLang == NULL)
      pLang=pDocLang;
    
    ASSERT (pLang);
    
    StartFoldMark   = pLang->m_SCom + pLang->m_SFold + "  ";
    EndFoldMark     = pLang->m_SCom + pLang->m_EFold + pLang->m_ECom;
    LazyEndFoldMark = pLang->m_SCom + pLang->m_EFold;
    
    int WarnedYet = 0;
    
    //}}}

    while (StripLine (RawLine, &Next)) //stripline returns next line with \n (even if one line)
    {
        //{{{  init next line
        
        RawLineLen = strlen (RawLine);
        
        //Length of string is always >= 1 because MFC adds \n to line
        //HACK what about empty file though
        ASSERT (RawLineLen >= 1);
        
        //handle arbitrary length lines
        if (RawLine[RawLineLen-1] != '\n')
        {
            //read a partial line; append to buffer and read again
            Buf += (const char *)RawLine;
            continue;
        }
        
        //Truncate to exclude trailing linefeed added by ReadString
        RawLine[RawLineLen - 1] = '\0';
        
        //Move line to CSentence object;
        Buf += (const char *)RawLine;
        
        //special case for empty line
        //HACK need to extend this to all white space really
        if (Buf.IsEmpty())
        {
            pCurrent->m_LineList.AddTail (new CTextLine);
            continue;
        }
        
        //expand tabs to spaces
        Buf.ExpandTabs (pLang->m_Tab);
        
        //compute white space indent
        int Indent = Buf.GetIndent();
        
        //remove FullIndent from line to make fold relative to parent
        if (Indent < FullIndent)
        {
            if (!WarnedYet)
            {
                WarningBox ("Encountered line(s) not indented as far as parent fold.\n\n"
                            "Inserting spaces.");
                WarnedYet++;
            }
        }
        
        else
        {
            Indent -= FullIndent;
            Buf = Buf.Right (Buf.GetLength() - FullIndent);
        }
        
        pNewFold = NULL;
        ASSERT (Indent >= 0);
        
        //}}}

        if ((StartAt = Buf.Find (StartFoldMark)) == Indent)
        {
            //{{{  found start of fold
            
            pNewFold = new CFoldLine;
            ASSERT (pNewFold);
            MEM (pNewFold);
            
            int StartTitle = StartAt + StartFoldMark.GetLength();
            int SizeTitle  = Buf.GetLength() - StartTitle;
            
            SizeTitle -= pLang->m_ECom.GetLength();
            
            if (SizeTitle > 0)
                Buf = Buf.Mid (StartTitle, SizeTitle);
            else
                Buf = "";
            
             //set parent
            pNewFold->m_pParent = pCurrent;
            
            //set indent relative to parent
            pNewFold->m_Indent = Indent;
            
            //update FullIndent
            FullIndent += Indent;
            
            //Fill in fold title from updated Buf (IsStartFoldP does this)
            pNewFold->m_Body = Buf;
            
            //append new fold to children of current and make current
            pCurrent->m_LineList.AddTail (pNewFold);
            pCurrent = pNewFold;
            
            Buf.Empty();
            
            //}}}
            continue;
        }

        if ((StartAt = Buf.Find (LazyEndFoldMark)) == Indent)
        {
            //{{{  found end of fold
            
            if (!pCurrent->m_pParent)
            
                WarningBox ("Inconsistent fold structure while reading file.\n"
                            "(Too many end fold markers.)\n\n"
                            "Ignoring extra end fold marker.");
            
            else
            {
                //restore parent fold indent
                FullIndent -= pCurrent->m_Indent;
            
                //go back to parent
                pCurrent = pCurrent->m_pParent;
            }
            
            Buf.Empty();
            
            //}}}
            continue;
        }

        //{{{  must be a text line
        
        CTextLine *pTextLine = new CTextLine (Buf);
        MEM(pTextLine);
        
        pCurrent->m_LineList.AddTail (pTextLine);
        
        Buf.Empty();
        //}}}
    }
}
//}}}
//{{{  RemoveTrailingWhite

void CSentence::RemoveTrailingWhite ()
{
    *this = this->Left(FindTrailingWhite());
}
//}}}
//{{{  ExpandTabs

void CSentence::ExpandTabs (const int TabSize, const char TabChar, const char RepChar)
{
	if (TabSize > 0)
    {
      CSentence SenOut;
      CString TabStr (RepChar, TabSize);

      for (int Next = 0; Next < GetLength(); Next++)
          if (GetAt(Next) == TabChar)
            SenOut += TabStr.Left(TabSize-(Next%TabSize));
          else
            SenOut += GetAt(Next);

      *this = SenOut;
	}
}
//}}}
//{{{  GetIndent

int CSentence::GetIndent (const char WhiteSpaceChar) const
{
    if (!GetLength())
        return 0;

    int Indent = 0;

    while (GetAt(Indent) == WhiteSpaceChar)
       {
           Indent++;

           if (Indent == GetLength())
               return Indent;
       }

       return Indent;
}
//}}}
//{{{  ExtractBetween

BOOL CSentence::ExtractBetween (const char Delim, CString &Extract)
{
    int Start = Find (Delim);
    if (Start < 0 )
        return FALSE;

    Extract = Right (GetLength() - Start - 1);

    int End = Extract.Find (Delim);
    if (End < 0)
        return FALSE;

    Extract = Extract.Left (End);

    if (Extract.IsEmpty())
        return FALSE;

    return TRUE;
}
//}}}
//{{{  ExtractBetween

BOOL CSentence::ExtractBetween (const char Delim1, const char Delim2, CString &Extract)
{
    int Start = Find (Delim1);
    if (Start < 0 )
        return FALSE;

    Extract = Right (GetLength() - Start - 1);

    int End = Extract.Find (Delim2);
    if (End < 0)
        return FALSE;

    Extract = Extract.Left (End);

    if (Extract.IsEmpty())
        return FALSE;

    return TRUE;
}
//}}}

