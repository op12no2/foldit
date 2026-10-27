//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd
// langlst.cpp
//

#include "stdafx.h"
#include "langlst.h"
#include "foldit.h"
#include "inifile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLanguageList

//{{{  CLanguageList::CLanguageList()
CLanguageList::CLanguageList()
{
}
//}}}
//{{{  CLanguageList::~CLanguageList()
CLanguageList::~CLanguageList()
{
        for (int i = 0; i < GetSize(); i++)
        {
               CLanguage *pLang = (CLanguage *)GetAt(i);

               delete pLang;
        }

        RemoveAll();
}
//}}}
//{{{  CLanguage *CLanguageList::GetLanguageFromName (const char *Name)
CLanguage *CLanguageList::GetLanguageFromName (const char *Name)
{
        for (int i = 0; i < GetSize(); i++)
               if (Name == ((CLanguage *)GetAt(i))->m_Name)
                     return (CLanguage *)GetAt(i);

    return (CLanguage *)NULL;
}
//}}}
//{{{  void CLanguageList::ReadFromIniFile()
void CLanguageList::ReadFromIniFile()
{

  //{{{  remove all existing styles.
  
  for (int i = 0; i < GetSize(); i++)
         delete (CLanguage *)GetAt(i);
  
  RemoveAll();
  
  ASSERT (GetSize() == 0);
  
  //}}}
  //array for extension list.  extensions are used to define the
  //other parts of the language in the .ini file.
  CStringArray Names;
  int          NumLang;

  //read the lang extensions from the .ini file.
  CIniFile::ReadSection ("Languages", &Names);

  //calculate the number of styles.
  NumLang = Names.GetSize();

  //{{{  load the remainder of the lang definitions from the .ini file
  
  //load the remainder of the lang definitions from the .ini file
  //may be 0 lang defs
  //Always add 'All Files' Language so that there is at least one lang.
  
  CString Str;
  
  for (int i = 0; i < NumLang; i++)
  {
    CString Name = Names[i];
  
    CLanguage *pNewLang = new CLanguage;
    Add(pNewLang);
  
    pNewLang->m_Name = Name;
  
    CIniFile::Read    (Name, "StartComment",  pNewLang->m_SCom);
    CIniFile::Read    (Name, "EndComment",    pNewLang->m_ECom);
    CIniFile::Read    (Name, "StartFoldMark", pNewLang->m_SFold);
    CIniFile::Read    (Name, "EndFoldMark",   pNewLang->m_EFold);
    CIniFile::ReadInt (Name, "Tab",           pNewLang->m_Tab);
    CIniFile::Read    (Name, "FileRef",       pNewLang->m_Ref);
    CIniFile::Read    (Name, "FileExt",       pNewLang->m_FExt);
  }
  
  //}}}
  //{{{  add "Plain Text" so that at least one language is defined.
  
  CLanguage *pPlainText;
  
  if (!(pPlainText = GetLanguageFromName("Plain Text")))
  {
     CLanguage *pAll = new CLanguage;
     Add(pAll);
  
     pAll->m_Name= "Plain Text";
     pAll->m_SCom="";
     pAll->m_ECom="";
     pAll->m_SFold="{{{";
     pAll->m_EFold="}}}";
     pAll->m_Tab=8;
     pAll->m_Ref="";
     pAll->m_FExt="";
     pAll->m_ReadOnly = TRUE;
  }
  
  else
  {
    pPlainText->m_ReadOnly = TRUE;
  }
  
  //}}}

  Names.RemoveAll();

  //{{{  move Plain text to end of list
  
  for (int i = 0; i < GetSize(); i++)
  {
    pPlainText = (CLanguage *)GetAt(i);
    if (pPlainText->m_Name == "Plain Text")
    {
      RemoveAt(i);
      break;
    }
  }
  
  Add (pPlainText);
  
  //}}}

}
//}}}
//{{{  void CLanguageList::WriteToIniFile()
void CLanguageList::WriteToIniFile()
{
  for (int i = 0; i < GetSize(); i++)
  {
    CLanguage *pLang = (CLanguage *)GetAt(i);

    if (pLang->m_Name == "Plain Text")
    { //don't write this one
      ;
    }

    else
    { //write all except plain text
      CIniFile::Write ("Languages", pLang->m_Name, "");

      CIniFile::Write (pLang->m_Name, "FileExt",       pLang->m_FExt);
      CIniFile::Write (pLang->m_Name, "FileRef",       pLang->m_Ref);
      CIniFile::Write (pLang->m_Name, "StartComment",  pLang->m_SCom);
      CIniFile::Write (pLang->m_Name, "EndComment",    pLang->m_ECom);
      CIniFile::Write (pLang->m_Name, "StartFoldMark", pLang->m_SFold);
      CIniFile::Write (pLang->m_Name, "EndFoldMark",   pLang->m_EFold);
      CIniFile::WriteInt (pLang->m_Name, "Tab",        pLang->m_Tab);
    }
  }
}

//}}}
//{{{  CLanguage *CLanguageList::New()
CLanguage *CLanguageList::New()
{
        CLanguage *pNewLang = new CLanguage;

        Add (pNewLang);
        return pNewLang;
}
//}}}
//{{{  BOOL CLanguageList::Delete(CLanguage *pDelLang)
BOOL CLanguageList::Delete(CLanguage *pDelLang)
{
        for (int i = 0; i < GetSize(); i++)
               if ((CLanguage *)GetAt(i) == pDelLang)
               {
                     RemoveAt (i);
                     delete pDelLang;
                     return TRUE;
               }

        return FALSE;
}
//}}}
//{{{  FindLanguage

CLanguage *CLanguageList::FindLanguage (CString Buf, CString Ext)
{
  CLanguage* pNextLang;
  CString Proof;
  int FoundAt;
  int Next;

  //{{{  look for <SC><SF>language<EF><EC> in identifier line
  
  for (Next = 0; Next < GetSize(); Next++)
  {
      pNextLang = (CLanguage *)GetAt(Next);
      ASSERT (pNextLang);
  
      Proof = pNextLang->m_SFold;
      if (Proof.GetLength() > 0 && Proof[0] > ' ')
      { //only do it if the language has start foldmarks
        Proof = pNextLang->m_EFold;
        if (Proof.GetLength() > 0 && Proof[0] > ' ')
        { //only do it if the language has end foldmarks
  
          Proof = pNextLang->m_SCom + pNextLang->m_SFold + pNextLang->m_Name + pNextLang->m_EFold + pNextLang->m_ECom;
  
          FoundAt = Buf.Find (Proof);
          if ((FoundAt > 0) && (Buf[FoundAt-1] > ' ')) continue;
  
          if ((FoundAt >= 0) && (FoundAt < 64))  //must be very near the start of the file
          {
            return pNextLang;
          }
  
        }
      }
  }
  
  //}}}
  //{{{  look for <SC><SF><EF><EC> in identifier line
  
  for (Next = 0; Next < GetSize(); Next++)
  {
      pNextLang = (CLanguage *)GetAt(Next);
      ASSERT (pNextLang);
  
      Proof = pNextLang->m_SFold;
      if (Proof.GetLength() > 0 && Proof[0] > ' ')
      { //only do it if the language has start foldmarks
        Proof = pNextLang->m_EFold;
        if (Proof.GetLength() > 0 && Proof[0] > ' ')
        { //only do it if the language has end foldmarks
  
          Proof = pNextLang->m_SCom + pNextLang->m_SFold + pNextLang->m_EFold + pNextLang->m_ECom;
  
          FoundAt = Buf.Find (Proof);
          if ((FoundAt > 0) && (Buf[FoundAt-1] > ' ')) continue;
  
          if ((FoundAt >= 0) && (FoundAt < 64))  //must be very near the start of the file
          {
            return pNextLang;
          }
  
        }
      }
  }
  
  //}}}
  //{{{  relax constraint and look for <SC><SF>
  
  for (Next = 0; Next < GetSize(); Next++)
  {
      pNextLang = (CLanguage *)GetAt(Next);
      ASSERT (pNextLang);
  
      Proof = pNextLang->m_SFold;
      if (Proof.GetLength() > 0 && Proof[0] > ' ')
      { //only do it if the language has a startfoldmark
  
        Proof = pNextLang->m_SCom + pNextLang->m_SFold + "  ";
  
        FoundAt = Buf.Find (Proof);
        if ((FoundAt > 0) && (Buf[FoundAt-1] > ' ')) continue;
  
        if (FoundAt >= 0)
        {
            return pNextLang;
        }
      }
  }
  //}}}
  //{{{  relax constraint and look for <SC>
  
  for (Next = 0; Next < GetSize(); Next++)
  {
      pNextLang = (CLanguage *)GetAt(Next);
      ASSERT (pNextLang);
  
      Proof = pNextLang->m_SCom;
  
      if (Proof.GetLength() > 0 && Proof[0] > ' ')
      { //only do it if the language has a comment
  
        FoundAt = Buf.Find (Proof);
        if ((FoundAt > 0) && (Buf[FoundAt-1] > ' ')) continue;
  
        if (FoundAt >= 0)
        {
            return pNextLang;
        }
  
      }
  }
  //}}}
  //{{{  last resort - look at file extension (if got one)
  
  if (Ext.GetLength() > 0 && Ext[0] > ' ')
  {
    for (Next = 0; Next < GetSize(); Next++)
    {
        pNextLang = (CLanguage *)GetAt(Next);
        ASSERT (pNextLang);
  
        Proof = "." + pNextLang->m_FExt;
  
        if (Proof.GetLength() > 1 && Proof[1] > ' ')
        { //only do it if the language has an extension
  
          if (Proof == Ext)
          {
              return pNextLang;
          }
  
        }
    }
  }
  
  //}}}
  return NULL;  //can't find a lang

}

//}}}

