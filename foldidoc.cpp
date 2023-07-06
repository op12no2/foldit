//{{{}}}
// foldidoc.cpp : implementation of the CFolditDoc class
//

//{{{  Includes

#include "stdafx.h"
#include "foldit.h"
#include "foldidoc.h"
#include "inifile.h"
#include "langlst.h"
#include "sellang.h"
#include "ffile.h"

//}}}

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolditDoc

IMPLEMENT_DYNCREATE(CFolditDoc, CDocument)

//{{{  BEGIN_MESSAGE_MAP(CFolditDoc, CDocument)
BEGIN_MESSAGE_MAP(CFolditDoc, CDocument)
    //{{AFX_MSG_MAP(CFolditDoc)
    ON_COMMAND(ID_OPTIONS_LANGUAGES, OnOptionsLanguages)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGES, OnUpdateOptionsLanguages)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_LANG, OnUpdateIndicatorLang)
    ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
    ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}

/////////////////////////////////////////////////////////////////////////////
// CFolditDoc construction/destruction

//{{{  CFolditDoc::CFolditDoc()
CFolditDoc::CFolditDoc()
{
    m_TopFold.m_Body = "Top Level";
    m_pLanguage = NULL;
    m_Sticky = FALSE;
    m_FileStatus.m_mtime = 0;
}

//}}}
//{{{  CFolditDoc::~CFolditDoc()
CFolditDoc::~CFolditDoc()
{
}


//}}}
//{{{  void CFolditDoc::InitDoc()
void CFolditDoc::InitDoc()
{
  ;
}

//}}}
//{{{  BOOL CFolditDoc::OnNewDocument()
BOOL CFolditDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
       return FALSE;

    if (!m_pLanguage)
    {
       CSelLangDlg dlg (this);

       dlg.DoModal();

       if (!m_pLanguage)
       {
         TRACE0 ("Using default language\n");
         m_pLanguage = App()->m_pDefaultLanguage;
       }
    }

    //add one line so that editing can commence immediately
    m_TopFold.m_LineList.AddTail (new CTextLine);

    ASSERT (m_pLanguage);

    return TRUE;
}

//}}}
//{{{  BOOL CFolditDoc::OnOpenDocument(const char* pszPathName)
BOOL CFolditDoc::OnOpenDocument(const char* pszPathName)
{
    if (IsModified())
       TRACE0("CFolditDoc::OnOpenDocument: Warning: replacing an unsaved document\n");

    CFoldedFile    file;
    CFileException fe;

    //{{{  note path for later
    
    m_Folder = pszPathName;        //note as is for now
    
    char rext[_MAX_PATH];
    
    _splitpath(pszPathName,NULL,NULL,NULL,rext);
    
    m_Extension = rext;            //note extension (for possible language selection)
    
    char rpath[_MAX_PATH];
    
    if (_fullpath(rpath,pszPathName,_MAX_PATH) != NULL)
    {
      char rdrive[_MAX_DRIVE];
      char rdir[_MAX_DIR];
      _splitpath(rpath,rdrive,rdir,NULL,NULL);
      _makepath(rpath,rdrive,rdir,NULL,NULL);
      m_Folder = rpath;            //note full path to this doc
    }
    
    //}}}
    //{{{  open file
    if (!file.Open(pszPathName, CFile::modeRead | CFile::typeText, &fe))
    {
       //failed to open
       CString Msg (pszPathName);
       if (AfxMessageBox ("'"+Msg+"'"+" does not exist.\n\nCreate it now?", MB_YESNO) == IDYES)
       {
         if (!file.Open(pszPathName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText, &fe))
         {
             ReportSaveLoadException(pszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
                return FALSE;
         }
         else
         {
             //created file OK
             file.WriteString("\n");
             file.Close();
             file.Open(pszPathName, CFile::modeRead | CFile::typeText, &fe);
         }
       }
       else
       {
         //ReportSaveLoadException(pszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
             return FALSE;
       }
    }
    //}}}

    file.GetStatus(m_FileStatus);

    //get the file to determine the language and return a pointer to it
    m_pLanguage = file.SetLanguage(NULL,m_Extension);

    if (!m_pLanguage)
    {
       //CSelLangDlg dlg (this);

       //dlg.DoModal();

       if (!m_pLanguage)
       {
         TRACE0 ("Using default language\n");
         m_pLanguage = App()->m_pDefaultLanguage;
       }

       ASSERT (m_pLanguage);

       file.SetLanguage (m_pLanguage,m_Extension);
    }

    //opened OK; clear document
    DeleteContents();

    //and initialise
    InitDoc();

    SetModifiedFlag(TRUE);

    TRY
    {
       BeginWaitCursor();

       //read the file and fill the doc
       file.ReadFoldsTo (GetTop());

       file.Close();
    }

    CATCH_ALL(e)
    {
       //exception occured, abort open
       file.Abort();

       EndWaitCursor();

       ReportSaveLoadException(pszPathName, e,   FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);

        //clear up doc
        DeleteContents();

       return FALSE;
    }

    END_CATCH_ALL

    EndWaitCursor();

    SetPathName(pszPathName);

    //no edits yet
    SetModifiedFlag(FALSE);

    return TRUE;
}

//}}}
//{{{  BOOL CFolditDoc::OnSaveDocument(const char* pszPathName)
BOOL CFolditDoc::OnSaveDocument(const char* pszPathName)
{
    CFoldedFile file;
    CFileException fe;

    //open file for reading if it exists to check status
    if (file.Open(pszPathName, CFile::typeText | CFile::modeRead, &fe))
    {
        CFileStatus FileStatus;
        file.GetStatus(FileStatus);

        int Op = IDYES;

        if (FileStatus.m_mtime > m_FileStatus.m_mtime)
        {
            Op = AfxMessageBox ("This file has been changed by somebody"
                                " else while you have been editing it.\n\n"
                                "Overwrite with your changes?", MB_YESNOCANCEL);
        }

        if (Op == IDNO)
        {
           file.Close();
           return TRUE;
        }

        if (Op == IDCANCEL)
        {
            file.Close();
            return FALSE;
        }

        file.Close();
    }

    //re-open file for writing
    if (!file.Open(pszPathName,
                  CFile::modeCreate | CFile::typeText | CFile::modeReadWrite, &fe))
    {
       //failed
       ReportSaveLoadException(pszPathName, &fe, TRUE, AFX_IDP_INVALID_FILENAME);

       return FALSE;
    }

    TRY
    {
       BeginWaitCursor();

       //copy the doc language into the folded file object
       file.SetLanguage (m_pLanguage, m_Extension);

       //write the folds to file folded file
       file.WriteFoldsFrom(GetTop());

       file.Close();
       if (file.Open(pszPathName, CFile::typeText | CFile::modeRead, &fe))
       {
         //update the status
         file.GetStatus(m_FileStatus);
         file.Close();
       }
    }

    CATCH_ALL(e)
    {
       //exception occured, abort
       file.Abort();

       EndWaitCursor();

       ReportSaveLoadException(pszPathName, e, TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);

       return FALSE;
    }

    END_CATCH_ALL

    EndWaitCursor();

    //all edits accounted for now
    SetModifiedFlag(FALSE);

    return TRUE;
}

//}}}
//{{{  int CFolditDoc::GetViewCount()
int CFolditDoc::GetViewCount()
{
    int Count = 0;
    POSITION pos = GetFirstViewPosition();
    ASSERT (pos);
    while (pos)
    {
     GetNextView(pos);
     Count++;
    }

    return Count;
}

//}}}

/////////////////////////////////////////////////////////////////////////////
// CFolditDoc serialization

//{{{  void CFolditDoc::Serialize(CArchive& ar)
void CFolditDoc::Serialize(CArchive& ar)
{}

//}}}
//{{{  void CFolditDoc::DeleteContents()
void CFolditDoc::DeleteContents()
{
    //other stuff here
    CDocument::DeleteContents();  //Do Last
}

//}}}

/////////////////////////////////////////////////////////////////////////////
// CFolditDoc diagnostics

#ifdef _DEBUG
//{{{  void CFolditDoc::AssertValid() const
void CFolditDoc::AssertValid() const
{
    CDocument::AssertValid();
}

//}}}
//{{{  void CFolditDoc::Dump(CDumpContext& dc) const
void CFolditDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}

//}}}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFolditDoc commands

//{{{  void CFolditDoc::OnUpdateIndicatorLang(CCmdUI* pCmdUI)
void CFolditDoc::OnUpdateIndicatorLang(CCmdUI* pCmdUI)
{
    pCmdUI->SetText (m_pLanguage->m_Name);
    pCmdUI->Enable();
}

//}}}
//{{{  void CFolditDoc::OnOptionsLanguages()
void CFolditDoc::OnOptionsLanguages()
{
    CSelLangDlg dlg (this);

    dlg.DoModal();
}

//}}}
//{{{  void CFolditDoc::OnUpdateOptionsLanguages(CCmdUI* pCmdUI)
void CFolditDoc::OnUpdateOptionsLanguages(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  void CFolditDoc::OnFileClose()
void CFolditDoc::OnFileClose()
{
    CDocument::OnFileClose();
}

//}}}
//{{{  void CFolditDoc::OnUpdateFileClose(CCmdUI* pCmdUI)
void CFolditDoc::OnUpdateFileClose(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}

