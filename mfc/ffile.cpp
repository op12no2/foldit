// Copyright 1994,5,6 Leyfield Systems Ltd
// ffile.cpp: CFoldedFile class

//{{{  history
//
// 20/3/96  CRJ Folded.
// 21/3/96  CRJ Force Lazy end of file mode for speed.  This means we are
//              compatible with Origami and F end of folds and also fast.
// 3/4/97   CRJ Allow null length files.  NB. this hsppend by adding at least
//              one line to the file.  However if the file is closed without
//              typing anything then the file is not marked as dirty and it will
//              retain it's null length.
//

//}}}

//{{{  includes

#include "stdafx.h"
#include "mainfrm.h"
#include "foldit.h"
#include "foldline.h"
#include "textline.h"
#include "ffile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}
//{{{  SetLanguage

CLanguage *CFoldedFile::SetLanguage (CLanguage *pLang, CString Ext)
{
  //{{{  set from external source of specified

  char RawBuf[U_LANGSEARCHBUFFER];

  if (pLang)
  {
      m_pLanguage = pLang;
      return pLang;
  }
  //}}}
  //{{{  context
  
  CLanguage *pFoundLang;
  
  //}}}
  //{{{  read the head of the file
  
  if (!Read (RawBuf, U_LANGSEARCHBUFFER-1))
  {
      return NULL;
  }
  
  SeekToBegin();

  CString HeadOfFile(RawBuf);

  //}}}

  if ((pFoundLang = App()->m_pLanguages->FindLanguage (HeadOfFile, Ext)) != NULL)
    m_pLanguage = pFoundLang;

  return pFoundLang;  //may be NULL - triggers selection box or default selection
}

//}}}
//{{{  ReadFoldsTo

void CFoldedFile::ReadFoldsTo (CFoldLine *pFold)
{
    //{{{  context
    
    int StartAt;
    CFoldLine *pNewFold;
    CSentence Buf;
    char RawLine[U_FILEBUFFER];
    
    CFoldLine *pCurrent = pFold;
    
    int FullIndent = 0;
    int RawLineLen;
    
    CString StartFoldMark, EndFoldMark;
    
    BOOL UseMarks;
    
    //}}}
    //{{{  init
    
    ASSERT (m_pLanguage);
    
    StartFoldMark = m_pLanguage->m_SCom + m_pLanguage->m_SFold + "  ";
    EndFoldMark   = m_pLanguage->m_SCom + m_pLanguage->m_EFold;
    
    if (StartFoldMark[0] == ' ')
    { //folding on the basis of indentation level
      UseMarks = false;
    }
    else
    { //folding on the basis of fold marks
      UseMarks = true;
    }
    
    int WarnedYet = 0;
    
    //App()->LoadCursor(IDC_CURSORA);
    
    //}}}

    while (ReadString (RawLine, U_FILEBUFFER-1))
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
        Buf.ExpandTabs (m_pLanguage->m_Tab);
        
        //compute white space indent
        int Indent = Buf.GetIndent();
        
        //{{{  check for inconsistencies
        
        if (UseMarks && Indent < FullIndent)
        {
            if (!WarnedYet)
            {
                WarningBox ("Encountered line(s) not indented as far as parent fold.\n\n"
                            "Inserting spaces.");
                WarnedYet++;
            }
            Buf = CString(' ',FullIndent-Indent) + Buf;
            Indent = FullIndent;
        }
        
        else if (Indent < FullIndent)
        {  //when not using marks this is an end fold condition, so do nothing
            ;
        }
        
        //}}}
        
        pNewFold = NULL;
        ASSERT (Indent >= 0);
        
        //}}}

        //{{{  check for start of fold
        
        if (UseMarks)
        {
          StartAt = Buf.Find (StartFoldMark);
        }
        
        else if (Indent > FullIndent && Indent < Buf.GetLength())
        { //not using marks and indentation level has gone up
          StartAt = Indent;
        }
        
        else
        { //not a start fold
          StartAt = -1;
        }
        
        //}}}
        if (StartAt == Indent)
        {
            //{{{  found start of fold
            
            pNewFold = new CFoldLine;
            ASSERT (pNewFold);
            MEM (pNewFold);
            
            //set parent
            pNewFold->m_pParent = pCurrent;
            
            //set indent relative to parent
            pNewFold->m_Indent = Indent-FullIndent;
            
            //update FullIndent
            FullIndent += pNewFold->m_Indent;
            
            //setup fold title
            int StartTitle, SizeTitle;
            
            if (UseMarks)
            {
              StartTitle = StartAt + StartFoldMark.GetLength();
              SizeTitle  = Buf.GetLength() - StartTitle;
              SizeTitle -= m_pLanguage->m_ECom.GetLength();
            }
            else
            {
              StartTitle = StartAt;
              SizeTitle  = Buf.GetLength() - StartTitle;
            }
            
            //Fill in fold title
            if (SizeTitle > 0)
                pNewFold->m_Body = Buf.Mid (StartTitle, SizeTitle);
            else
                pNewFold->m_Body = "";
            
            //append new fold to children of current and make current
            pCurrent->m_LineList.AddTail (pNewFold);
            pCurrent = pNewFold;
            
            if (UseMarks)
            { //that's it
            }
            else
            { //the fold title is also part of the fold at the new level, so add it
              Buf = Buf.Right (Buf.GetLength() - FullIndent);
              CTextLine *pTextLine = new CTextLine (Buf);
              MEM(pTextLine);
            
              pCurrent->m_LineList.AddTail (pTextLine);
            
            }
            
            Buf.Empty();
            
            //}}}
            continue;
        }

        //{{{  check for end of fold
        
        if (UseMarks)
        {
          StartAt = Buf.Find (EndFoldMark);
        }
        
        else if (Indent < FullIndent)
        { //not using marks and indentation level has gone down
          StartAt = Indent;
        }
        
        else
        { //not and end fold condition
          StartAt = -1;
        }
        
        //}}}
        if (StartAt == Indent)
        {
            //{{{  found end of fold
            
            if (UseMarks && !pCurrent->m_pParent)
            
                WarningBox ("Inconsistent fold structure while reading file.\n"
                            "(Too many end fold markers.)\n\n"
                            "Ignoring extra end fold marker.");
            
            else if (UseMarks)
            {
                //restore parent fold indent
                FullIndent -= pCurrent->m_Indent;
            
                //go back to parent
                pCurrent = pCurrent->m_pParent;
            }
            
            else
            { //need to unwind the approp number of levels
              while (FullIndent > Indent)
              {
                //restore parent fold indent
                FullIndent -= pCurrent->m_Indent;
            
                //go back to parent
                pCurrent = pCurrent->m_pParent;
            
                ASSERT(pCurrent);
              }
            
              //the end fold line is part of the parent at its indent level
              Buf = Buf.Right (Buf.GetLength() - FullIndent);
              CTextLine *pTextLine = new CTextLine (Buf);
              MEM(pTextLine);
            
              pCurrent->m_LineList.AddTail (pTextLine);
            
            }
            
            Buf.Empty();
            
            //}}}
            continue;
        }

        //{{{  must be a text line
        
        //remove parent indentation
        Buf = Buf.Right (Buf.GetLength() - FullIndent);
        
        CTextLine *pTextLine = new CTextLine (Buf);
        MEM(pTextLine);
        
        pCurrent->m_LineList.AddTail (pTextLine);
        
        Buf.Empty();
        
        //}}}
    }

    //must be at least one line
    if (!pFold->m_LineList.GetCount())
    {
        CTextLine *pTextLine = new CTextLine();
        MEM(pTextLine);
        pFold->m_LineList.AddTail (pTextLine);
    }
}

//}}}
//{{{  WriteFoldsFrom

void CFoldedFile::WriteFoldsFrom (CFoldLine *pFold)
{
    CFoldLine *pCurrent = pFold;

    ASSERT (m_pLanguage);
    WriteFold (pCurrent, 0);
}
//}}}
//{{{  WriteFold

void CFoldedFile::WriteFold (CFoldLine *pFold, int CurIndent)
{

    //{{{  context
    
    CString CurIndentStr (' ', CurIndent);
    CSentence Str;
    
    CString StartFoldMark, EndFoldMark;
    
    BOOL UseMarks;
    
    //}}}
    //{{{  init
    
    StartFoldMark = m_pLanguage->m_SCom + m_pLanguage->m_SFold + "  ";
    EndFoldMark   = m_pLanguage->m_SCom + m_pLanguage->m_EFold;
    
    if (StartFoldMark[0] == ' ')
    { //folding on the basis of indentation level
      UseMarks = false;
    }
    else
    { //folding on the basis of fold marks
      UseMarks = true;
    }
    
    //}}}

    if (UseMarks)
    {
      Str = CurIndentStr + StartFoldMark;
      pFold->m_Body.RemoveTrailingWhite();
      Str += pFold->m_Body + m_pLanguage->m_ECom + "\n";

      if (pFold->m_pParent)
          WriteString (Str);
    }
    else
      Str = "";

    POSITION pos = pFold->m_LineList.GetHeadPosition();

    while (pos)
    {
        CLine *pLine = (CLine *)pFold->m_LineList.GetNext (pos);

        if (pLine->IsFoldP())
            WriteFold ((CFoldLine *)pLine, CurIndent + ((CFoldLine *)pLine)->m_Indent);

        else
        {
            pLine->m_Body.RemoveTrailingWhite();
            //WriteString (CurIndentStr + AddIndentStr + pLine->m_Body + "\n");
            WriteString (CurIndentStr + pLine->m_Body + "\n");
        }
    }

    if (UseMarks)
    {
      Str = CurIndentStr + EndFoldMark + m_pLanguage->m_ECom + "\n";

      if (pFold->m_pParent)
          WriteString (Str);
    }

}

//}}}

