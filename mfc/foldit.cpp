// foldit.cpp: CFolditApp

//{{{  history
//
// 3/4/97   CRJ And m_EndFoldCue
// 13/5/97  CRJ Add serialno init/save
// 20/11/99 DCN Allow for long file names in the file associations
//              Add setup of foldit file types
//              Look for file associations in the registry too
//              Suppress splash window
// 27/11/99 DCN Look for file associations in the registry before win.ini
// 1/12/99  DCN Dump m_LastRed (replaced by m_Folder in CFolditDoc)
// 27/06/07 DCN Look for the INI file file via an env var (see InitInstance)
//
//}}}
//{{{  includes

#include "stdafx.h"
#include "foldit.h"
#include "foldline.h"
#include "mainfrm.h"
#include "foldidoc.h"
#include "pastelis.h"
#include "foldivw.h"
#include "inifile.h"
#include "language.h"
#include "miscopts.h"



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}
//{{{  MESSAGE_MAP

BEGIN_MESSAGE_MAP(CFolditApp, CWinApp)
    //{{AFX_MSG_MAP(CFolditApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
    ON_COMMAND(ID_HELP_TUTORIAL, OnHelpTutorial)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
    ON_COMMAND(ID_EDIT_RESTORE, OnEditRestore)
    ON_UPDATE_COMMAND_UI(ID_EDIT_RESTORE, OnUpdateEditRestore)
    ON_COMMAND(ID_OPTIONS_MISCELLANEOUS, OnOptionsMiscellaneous)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_MISCELLANEOUS, OnUpdateOptionsMiscellaneous)
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
    // Global help commands
    ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
    ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
END_MESSAGE_MAP()

//}}}
//{{{  FolditApp()

CFolditApp::CFolditApp()
{
    m_pwndSplash = NULL;
    m_pLanguages = new CLanguageList;
    MEM(m_pLanguages);
    m_pPasteFold = NULL;
    m_pPasteDoc  = NULL;
}

//}}}
//{{{  theApp

CFolditApp NEAR theApp;

//}}}
//{{{  InitInstance

BOOL CFolditApp::InitInstance()
{
    //{{{  data
    
    WINDOWPLACEMENT place;
    place.length = sizeof(place);
    
    //}}}
    //{{{  load standard MFC ini stuff
    
    //Standard initialization
    //If you are not using these features and wish to reduce the size
    //of your final executable, you should remove from the following
    //the specific initialization routines you do not need.
    
    LoadStdProfileSettings();
    
    //}}}
    //{{{  create splash window
    
    m_dwSplashTime = 0;
    
    ASSERT(m_pwndSplash == NULL);
    
    //21/11/99 DCN Suppress splash window
    //m_pwndSplash = new CSplashWnd;
    //MEM(m_pwndSplash);
    //
    //m_pwndSplash->Create();
    //m_pwndSplash->ShowWindow(SW_SHOW);
    //m_pwndSplash->UpdateWindow();
    //
    //ASSERT(m_pwndSplash != NULL);
    
    //}}}
    //{{{  init macro facility
    
    m_GotTime        = FALSE;
    m_KeyMacroActive = FALSE;
    
    //}}}
    //{{{  tweak the MFC profile var to point at our ini file
    
    //AfxMessageBox(m_pszHelpFilePath);
    //AfxMessageBox(m_pszExeName);
    //AfxMessageBox(m_pszProfileName);
    
    m_ExePath           = m_pszHelpFilePath;
    m_ExePath           = m_ExePath.Left(m_ExePath.GetLength() - (strlen(m_pszProfileName)+1));
    m_pSavedProfileName = m_pszProfileName;
    
    char *iniat = getenv((const char *)m_pszProfileName);
    //char *iniat = getenv("foldit.ini");
    //AfxMessageBox(iniat);
	if (iniat != NULL)
      CIniFile::m_IniName = iniat;
    else
      CIniFile::m_IniName = m_ExePath + "\\" + m_pszProfileName;
    
    strcpy (m_ProfileStr, (const char *)CIniFile::m_IniName);
    
    m_pszProfileName = m_ProfileStr;
    
    TRACE1 ("ini file=%s\n", m_pszProfileName);
    
    //AfxMessageBox(m_pszProfileName);
    
    //}}}
    //{{{  register 3D controls
    
    //Ctl3dRegister     (AfxGetInstanceHandle());
    //Ctl3dAutoSubclass (AfxGetInstanceHandle());
    
    //SetDialogBkColor();
    
    //}}}
    //{{{  init paste buffer
    
    m_MaxPaste = 10;
    
    CIniFile::ReadInt("Defaults", "PasteBufferLen", m_MaxPaste);
    
    //}}}
    //{{{  register doc templates
    
    //Register the application's document templates.  Document templates
    //serve as the connection between documents, frame windows and views.
    
    AddDocTemplate(new CMultiDocTemplate(IDR_FOLDITTYPE,
            RUNTIME_CLASS(CFolditDoc),
            RUNTIME_CLASS(CFolditMDIChildWnd),
            RUNTIME_CLASS(CFolditView)));
    
    //}}}
    //{{{  read defaults from ini file
    
    //Read the fold line prefix from the .ini file it exists
    
    CIniFile::Read ("Defaults", "FoldPrefix", CFoldLine::m_Prefix);
    
    //Maximising views
    m_MaxViews=FALSE;
    CIniFile::ReadInt ("Defaults", "MaximiseViews", m_MaxViews);
    
    //Navigation Stuff
    m_NavEnterRefP  = TRUE;
    m_NavAutoCloseP = FALSE;
    
    CIniFile::ReadInt ("Defaults", "UseOpenFileRefs", m_NavEnterRefP);
    CIniFile::ReadInt ("Defaults", "AutoCloseFile",   m_NavAutoCloseP);
    
    m_FileFilter = "All Files (*.*)|*.*||";
    CIniFile::Read ("Defaults", "FileFilter", m_FileFilter);
    
    //last search string
    m_SearchStr = "";
    CIniFile::Read ("Defaults", "SearchString", m_SearchStr);
    
    //end fold cue
    m_EndFoldCue = "-----";
    CIniFile::Read ("Defaults", "EndFoldCue", m_EndFoldCue);
    
    //Sig etc
    m_DateTag = "Sig";
    CIniFile::Read ("Defaults", "DateTag", m_DateTag);
    m_SerialNo = 0;
    CIniFile::ReadUInt ("Defaults", "SerialNo", m_SerialNo);
    m_SerialFmt = "%05u";
    CIniFile::Read ("Defaults", "SerialFmt", m_SerialFmt);
    
    //Fold tails
    m_DelFoldTails = FALSE;
    CIniFile::ReadInt ("Defaults", "DelFoldTails", m_DelFoldTails);
    
    //last open file filter index
    m_ofn.nFilterIndex = 0;
    int nFilter;
    CIniFile::ReadInt   ("Defaults", "FilterIndex", nFilter);
    m_ofn.nFilterIndex = nFilter;
    
    //rip behaviour
    m_AlwaysRip = 0;
    CIniFile::ReadInt ("Defaults", "AlwaysRip", m_AlwaysRip);
    
    if (m_AlwaysRip && m_MaxViews)
      m_AlwaysRip = 0;
    
    //background
    int r,g,b;
    
    r=192;
    g=192;
    b=192;
    
    CIniFile::ReadInt ("Defaults", "BackRed", r);
    CIniFile::ReadInt ("Defaults", "BackGreen", g);
    CIniFile::ReadInt ("Defaults", "BackBlue", b);
    
    m_DefBkGnd = RGB(r,g,b);
    
    //default font
    
    r=0;
    g=0;
    b=255;
    
    CIniFile::ReadInt ("Defaults", "ForeRed", r);
    CIniFile::ReadInt ("Defaults", "ForeGreen", g);
    CIniFile::ReadInt ("Defaults", "ForeBlue", b);
    
    m_DefFrGnd = RGB(r,g,b);
    
    m_DefaultFontName = "Courier";
    CIniFile::Read   ("Defaults", "FontName",   m_DefaultFontName);
    
    m_DefaultFontHeight = 0;
    CIniFile::ReadInt("Defaults", "FontHeight", m_DefaultFontHeight);
    
    //}}}
    //{{{  default language list
    
    m_pLanguages->ReadFromIniFile();
    
    m_pDefaultLanguage = m_pLanguages->GetLanguageFromName ("Plain Text");
    
    ASSERT (m_pDefaultLanguage);
    
    //}}}
    //{{{  win.ini extension list and ini file redirection dirs
    
    //read foldit file types
    CIniFile::ReadSection ("FolditFileTypes", &m_FolditFileTypes);
    
    //read redirection list
    CIniFile::ReadSection ("Redirection", &m_Red);
    
    //read associations from win.ini
    CString OldIniFile (CIniFile::m_IniName);
    CIniFile::m_IniName = "win.ini";
    CIniFile::ReadSection ("Extensions", &m_AssocExt);
    int i;
    CString ThisApp;
    
    for (i=0; i < m_AssocExt.GetSize(); i++)
    {
       CIniFile::Read ("Extensions", m_AssocExt[i], ThisApp);
       m_Assoc.Add(ThisApp);
       m_Assoc[i] = m_Assoc[i].Mid(0, m_Assoc[i].Find('^'));
    }
    
    CIniFile::m_IniName = OldIniFile;
    
    //}}}
    //{{{  create the main frame window
    
    CMainFrame* pMainFrame = new CMainFrame;
    MEM(pMainFrame);
    
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    
    if (pMainFrame->GetWindowPlacement(&place))
    {
      CIniFile::ReadInt ("Defaults", "MainWinLeft",   (int&)place.rcNormalPosition.left);
      CIniFile::ReadInt ("Defaults", "MainWinRight",  (int&)place.rcNormalPosition.right);
      CIniFile::ReadInt ("Defaults", "MainWinTop",    (int&)place.rcNormalPosition.top);
      CIniFile::ReadInt ("Defaults", "MainWinBottom", (int&)place.rcNormalPosition.bottom);
      CIniFile::ReadUInt("Defaults", "MainWinShow",   place.showCmd);
    
      pMainFrame->SetWindowPlacement(&place);
      pMainFrame->ShowWindow(place.showCmd);
      pMainFrame->UpdateWindow();
    }
    else
    {
      pMainFrame->ShowWindow(m_nCmdShow);
      pMainFrame->UpdateWindow();
    }
    
    m_pMainWnd = pMainFrame;
    ((CMainFrame *)(m_pMainWnd))->m_wndFindBox.SetWindowText(m_SearchStr);
    ((CMainFrame *)(m_pMainWnd))->UpdateFindEnv();
    
    //OnFileNew();
    
    //}}}
    //{{{  enable file manager drag/drop
    
    m_pMainWnd->DragAcceptFiles();
    EnableShellOpen();
    RegisterShellFileTypes();
    //}}}
    //{{{  parse command line
    
    if (m_lpCmdLine[0] == '\0')
    {
        // create a new (empty) document
        //OnFileNew();
    }
    else if ((m_lpCmdLine[0] == '-' || m_lpCmdLine[0] == '/') &&
        (m_lpCmdLine[1] == 'e' || m_lpCmdLine[1] == 'E'))
    {
        // program launched embedded - wait for DDE or OLE open
    }
    else
    {
        // open an existing document
        CFolditDoc *pDoc = (CFolditDoc *)OpenDocumentFile(m_lpCmdLine);
        if (pDoc)
            pDoc->m_Sticky = TRUE;
    }
    
    //}}}
    //{{{  init splash timer
    
    m_dwSplashTime = ::GetCurrentTime();
    
    //}}}

    m_InitFind = "";
    m_pLastFind = NULL;

    return TRUE;
}

//}}}
//{{{  ExitInstance
int CFolditApp::ExitInstance()
{
    //m_pMainWnd is NULL at this point

    //{{{  deregister 3D controls
    
    //Ctl3dUnregister(AfxGetInstanceHandle());
    //}}}
    //{{{  write defaults to ini file
    
    CIniFile::WriteInt  ("Defaults", "DelFoldTails",    m_DelFoldTails);
    CIniFile::WriteInt  ("Defaults", "MaximiseViews",   m_MaxViews);
    CIniFile::WriteInt  ("Defaults", "UseOpenFileRefs", m_NavEnterRefP);
    CIniFile::WriteInt  ("Defaults", "AutoCloseFile",   m_NavAutoCloseP);
    CIniFile::Write     ("Defaults", "FontName",        m_DefaultFontName);
    CIniFile::WriteInt  ("Defaults", "FontHeight",      m_DefaultFontHeight);
    CIniFile::WriteInt  ("Defaults", "ForeRed",         GetRValue(m_DefFrGnd));
    CIniFile::WriteInt  ("Defaults", "ForeGreen",         GetGValue(m_DefFrGnd));
    CIniFile::WriteInt  ("Defaults", "ForeBlue",         GetBValue(m_DefFrGnd));
    //CIniFile::WriteUInt ("Defaults", "Background",      m_DefBkGnd);
    CIniFile::Write     ("Defaults", "DateTag",         m_DateTag);
    //CIniFile::Write    ("Defaults", "FileFilter",      m_FileFilter);
    CIniFile::Write     ("Defaults", "FoldPrefix",      CFoldLine::m_Prefix);
    CIniFile::WriteInt  ("Defaults", "FilterIndex",     (int)m_ofn.nFilterIndex);
    CIniFile::WriteInt  ("Defaults", "PasteBufferLen",  m_MaxPaste);
    CIniFile::WriteInt  ("Defaults", "AlwaysRip",       m_AlwaysRip);
    CIniFile::WriteUInt ("Defaults", "SerialNo",        m_SerialNo);
    CIniFile::Write     ("Defaults", "SerialFmt",       m_SerialFmt);
    CIniFile::Write     ("Defaults", "SearchString",    m_SearchStr);
    CIniFile::Write     ("Defaults", "EndFoldCue",      m_EndFoldCue);
    
    CString Var;
    if (m_ofn.lpstrInitialDir != NULL)
    {
        Var = (const char *)m_ofn.lpstrInitialDir;
        CIniFile::Write ("Defaults", "InitDir", Var);
    }
    
    //}}}
    //{{{  write languages to ini file
    
    m_pLanguages->WriteToIniFile();
    
    delete m_pLanguages;
    
    //}}}

    BOOL ret = CWinApp::ExitInstance();
    m_pszProfileName = m_pSavedProfileName;
    return ret;
}

//}}}

//{{{  CopyToClipboard

void CFolditApp::CopyToClipboard(CLanguage *pLang)
{
    HGLOBAL hglb;
    char FAR *lpvBuffer;
    CString ClipboardString;

    m_pPasteFold->WriteFoldsToString (m_pPasteFold, ClipboardString, pLang);

    if (!(hglb = GlobalAlloc(GMEM_MOVEABLE, ClipboardString.GetLength())))
    {
        AfxMessageBox ("Cannot allocate memory to write to clipboard.");
        return;
    }
    if (!(lpvBuffer = (char FAR *)GlobalLock(hglb)))
    {
           AfxMessageBox ("Cannot lock allocated memory for clipboard.");
           GlobalFree(hglb);
           return;
    }

    lstrcpy(lpvBuffer, (const char *)ClipboardString);
    GlobalUnlock(hglb);

    if (::OpenClipboard(App()->m_pMainWnd->GetSafeHwnd()))
    {
        ::EmptyClipboard();
        ::SetClipboardData (CF_TEXT, hglb);
        ::CloseClipboard();
    }
    else
    {
        AfxMessageBox("Failed to open clip board - it's in use by another application.");
    }
    return;
}

//}}}
//{{{  PasteFromClipboard

int CFolditApp::PasteFromClipboard(CLanguage *pLang)
{
    HANDLE hClipData;
    char FAR *lpClipData;
    CSentence ClipboardString;

    if (OpenClipboard(App()->m_pMainWnd->GetSafeHwnd()))
    {
        if (!IsClipboardFormatAvailable(CF_TEXT))
        {
            CloseClipboard();
            return FALSE;
        }

        if (pLang == NULL)
        {
            CloseClipboard();
            return TRUE;
        }

         hClipData  = GetClipboardData(CF_TEXT);
         lpClipData = (char FAR *)GlobalLock(hClipData);

         lstrcpy ((char *)ClipboardString.GetBuffer(GlobalSize(hClipData)),lpClipData);

        ClipboardString.ReleaseBuffer();
        //AfxMessageBox(ClipboardString);
        GlobalUnlock(hClipData);
        CloseClipboard();
    }

    CFoldLine *pPaste = NewPaste("Clipboard Data");
    ClipboardString.ReadFoldsTo(pPaste, pLang);
    return TRUE;
}

//}}}
//{{{  Exec
//
// redirection - is it a foldit file - if not then execute it
// a file is not executable if: its extension is a memeber of the m_FolditFileTypes
// a file is executable if: its extension is in m_AssocExt
//                          there is a class association in the registry
// a file is not executable otherwise
//

int CFolditApp::Exec (CString File)
{
    CString CmdLine;
    int i;

    //{{{  pass 1 - is its extension in m_FolditFileTypes
    for (i=0; i < m_FolditFileTypes.GetSize(); i++)
    {
      if (m_FolditFileTypes[i].CompareNoCase(File.Right(3)) == 0)
      {
         return 0;   //this is a 'native' file, don't execute
      }
    }
    //}}}
    //{{{  pass 2 - is there a class in the registry
    
    CString fExt = File.Right(4);
    fExt.MakeUpper();
    
    CString ClassNameKey;
    CString AppName = "";
    
    char ValStr[300];
    long ValSize = 300;
    HKEY ValKey;
    
    if( RegQueryValue(HKEY_CLASSES_ROOT,fExt,ValStr,&ValSize) == 0 )
    {
      //{{{  got a class
      
      ClassNameKey = ValStr;
      if( ClassNameKey.IsEmpty() )
        ;
      else
      {
        ClassNameKey += "\\shell\\open\\command\\";
        if( RegOpenKeyEx(HKEY_CLASSES_ROOT,ClassNameKey,NULL,KEY_ALL_ACCESS,&ValKey) == 0 )
        {
          ValSize = 300;
          if( RegQueryValue(ValKey,NULL,ValStr,&ValSize) == 0 )
          {
            AppName = ValStr;
            if( ~AppName.IsEmpty() )
            {
              //got one
              RegCloseKey(ValKey);
              //strip out %1 or "%1"
              int eof = AppName.Find("\"%1\"");
              if( eof > 0 )
                AppName = AppName.Left(eof);
              eof = AppName.Find("%1");
              if( eof > 0 )
                AppName = AppName.Left(eof);
              CmdLine = AppName + " \"" + File + "\"";
              WinExec ((const char *)CmdLine, SW_SHOW);
              return 1;
            }
          };
          RegCloseKey(ValKey);
        }
      }
      
      //}}}
    }
    
    //}}}
    //{{{  pass 3 - is its extension in m_AssocExt
    for (i=0; i < m_Assoc.GetSize(); i++)
    {
      if (m_AssocExt[i].CompareNoCase(File.Right(3)) == 0)
      {
         CmdLine = m_Assoc[i] + " \"" + File + "\"";
         WinExec ((const char *)CmdLine, SW_SHOW);
         return 1;
      }
    }
    //}}}

    return 0;  //none of the above, so don't execute
}
//}}}
//{{{  NewPaste

CFoldLine *CFolditApp::NewPaste(CString Title)
{
    CTime t = CTime::GetCurrentTime();
    //HACK ignore title so far
    Title = t.Format("%H:%M ");
    m_pPasteFold = new CFoldLine;
    MEM(m_pPasteFold);
    m_pPasteFold->m_Body = Title;
    m_pPasteFold->m_pParent = &m_PasteStack;
    m_PasteStack.m_LineList.AddTail (m_pPasteFold);

    if (m_PasteStack.m_LineList.GetCount() > m_MaxPaste)
    {
        POSITION pos = m_PasteStack.m_LineList.GetHeadPosition();
        delete m_PasteStack.m_LineList.GetAt(pos);
        m_PasteStack.m_LineList.RemoveAt(pos);
    }
    return m_pPasteFold;
}

//}}}
//{{{  OnAppAbout
void CFolditApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

//}}}
//{{{  OnFileOpen

void CFolditApp::OnFileOpen()
{
    CString newName;
    if (!DoPromptFileName(newName, OFN_FILEMUSTEXIST, TRUE, NULL))
        return;

    CWinApp::OpenDocumentFile(newName);
}
//}}}
//{{{  DoPromptFileName

BOOL CFolditApp::DoPromptFileName(CString& fileName,
                                     DWORD lFlags,
                                         BOOL bOpenFileDialog,
                                 CDocTemplate *pTemplate)
{
    //construct title of file dialog
    CString title;

    if (bOpenFileDialog)
        title = "Open Folded File";
    else
        title = "Save Folded File";

    //construct filter string
    CString strFilter (m_FileFilter);

    //create file open/save dialog
    CFileDialog dlgFile(bOpenFileDialog,  //open(T) or save(F)
                          NULL,
                        NULL,
                        OFN_OVERWRITEPROMPT | lFlags,
                        strFilter);

    dlgFile.m_ofn.nFilterIndex = m_ofn.nFilterIndex;
    dlgFile.m_ofn.hwndOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
    dlgFile.m_ofn.lpstrTitle = title;
    dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

    BOOL bRet = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
    fileName.ReleaseBuffer();

    m_ofn = dlgFile.m_ofn;
    return bRet;
}

//}}}
//{{{  OnUpdateFileOpen

void CFolditApp::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
    pCmdUI->Enable();
}

//}}}
//{{{  OnIdle

BOOL CFolditApp::OnIdle(LONG lCount)
{
        BOOL bMore = CWinApp::OnIdle(lCount);
        if (m_pwndSplash != NULL)
        {
           //{{{  manage splash window if it still exists
           
           if (m_pwndSplash->m_hWnd != NULL)
           {
               if (::GetCurrentTime() - m_dwSplashTime >= SPLASH_TIME)
               {
                   m_pwndSplash->DestroyWindow();
                   delete m_pwndSplash;
                   m_pwndSplash = NULL;
                   m_pMainWnd->UpdateWindow();
               }
           }
           else
              m_pwndSplash = NULL; //window went without us knowing!
           
           //}}}
        }

        //{{{  junk
        //if (bResult && (!m_GotTime) && m_pwndSplash == NULL)
        //{
        //    m_dwSplashTime = ::GetCurrentTime();
        //    m_GotTime      = TRUE;
        //     return TRUE;
        //}
        //
        //if (bResult && m_GotTime && m_pwndSplash == NULL && ((::GetCurrentTime() - m_dwSplashTime) >= 1000))
        //{
        //    //MessageBeep(-1);
        //    m_GotTime = FALSE;
        //    return FALSE;
        //}
        
        //}}}

        if (bMore) return bMore;
        if (m_pwndSplash) return TRUE;
        return FALSE;
}

//}}}
//{{{  PreTranslateMessage

BOOL CFolditApp::PreTranslateMessage(MSG* pMsg)
{
    if (m_pwndSplash != NULL)
    {
        if (pMsg->message == WM_KEYDOWN ||
            pMsg->message == WM_SYSKEYDOWN ||
            pMsg->message == WM_LBUTTONDOWN ||
            pMsg->message == WM_RBUTTONDOWN ||
            pMsg->message == WM_MBUTTONDOWN ||
            pMsg->message == WM_NCLBUTTONDOWN ||
            pMsg->message == WM_NCRBUTTONDOWN ||
            pMsg->message == WM_NCMBUTTONDOWN)
        {
            if (pMsg->hwnd == m_pwndSplash->m_hWnd) m_dwSplashTime -= SPLASH_TIME;
            else
            {
                m_pwndSplash->DestroyWindow();
                delete m_pwndSplash;
                m_pwndSplash = NULL;
                m_pMainWnd->UpdateWindow();
            }
        }
    }

    return CWinApp::PreTranslateMessage(pMsg);
}
//}}}
//{{{  OnHelpTutorial

void CFolditApp::OnHelpTutorial()
{
    CString Tutorial ("\\tutorial.txt");

    OpenDocumentFile(m_ExePath+Tutorial);
}

//}}}
//{{{  OnEditUnDo

void CFolditApp::OnEditUndo()
{
    CFoldLine *pOldBuffer = m_pPasteFold;

    CPasteListDlg dlg (&m_PasteStack, &m_pPasteFold);

    dlg.DoModal();

    //if (pOldBuffer != m_pPasteFold)          //dont copy to clipboard
    //  CopyToClipboard(m_pDefaultLanguage);
}

//}}}
//{{{  OnUpdateEditUndo

void CFolditApp::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  OnEditRestore

void CFolditApp::OnEditRestore()
{
    OnEditUndo();
}

//}}}
//{{{  OnUpdateEditRestore

void CFolditApp::OnUpdateEditRestore(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_PasteStack.m_LineList.GetCount() != 0);
}
//}}}

//{{{  CFolditMDIChildWnd (class def)

//{{{  IMPLEMENT_DYNCREATE

IMPLEMENT_DYNCREATE(CFolditMDIChildWnd, CMDIChildWnd)
//}}}
//{{{  CFolditMDIChildWnd

CFolditMDIChildWnd::CFolditMDIChildWnd()
{
}

//}}}
//{{{  CFolditMDIChildWnd

CFolditMDIChildWnd::~CFolditMDIChildWnd()
{
}
//}}}
//{{{  MESSAGE_MAP

BEGIN_MESSAGE_MAP(CFolditMDIChildWnd, CMDIChildWnd)
    //{{AFX_MSG_MAP(CFolditMDIChildWnd)
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  OnKillFocus

void CFolditMDIChildWnd::OnKillFocus(CWnd* pNewWnd)
{
    CMDIChildWnd::OnKillFocus(pNewWnd);

    CFolditDoc *pDoc   = (CFolditDoc *)GetActiveDocument();

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos)
    {
        CFolditView *pView = (CFolditView *)pDoc->GetNextView(pos);

        //Set title to fold title
        pView->GetParent()->SetWindowText (pDoc->GetTitle() + " :: " + pView->m_pCurrent->m_Body);
    }
}

//}}}
//{{{  OnSetFocus

void CFolditMDIChildWnd::OnSetFocus(CWnd* pOldWnd)
{
    CMDIChildWnd::OnSetFocus(pOldWnd);

    CFolditDoc *pDoc   = (CFolditDoc *)GetActiveDocument();

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos)
    {
           CFolditView *pView = (CFolditView *)pDoc->GetNextView(pos);

        //Set title to fold title
        pView->GetParent()->SetWindowText (pDoc->GetTitle() + " :: " + pView->m_pCurrent->m_Body);
    }

}

//}}}
//{{{  PreCreateWindow

BOOL CFolditMDIChildWnd::PreCreateWindow (CREATESTRUCT &cs)
{
    cs.style &= ~(LONG)FWS_ADDTOTITLE;

    if (App()->m_MaxViews)
    {
      cs.style |= WS_MAXIMIZE;
      //cs.style |= WS_VISIBLE; //GPF
    }
    else
    {
      cs.style &= ~WS_MAXIMIZE;
    }

    return CMDIChildWnd::PreCreateWindow(cs);
}

//}}}
//{{{  OnCtlColor

HBRUSH CFolditMDIChildWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    TRACE0 ("CFolditMDIChildWnd::OnCtlColor()\n");

    if (!GetView())
    {
        return CMDIChildWnd::OnCtlColor(pDC, pWnd, nCtlColor);
    }

    pDC->SetTextColor(GetView()->m_cf.rgbColors);
    pDC->SetBkColor(GetView()->m_BkColor);

    switch (nCtlColor)
    {
        case CTLCOLOR_EDIT:
            return (HBRUSH)(GetView()->m_BkBrush.GetSafeHandle());

        default:
            return CMDIChildWnd::OnCtlColor(pDC, pWnd, nCtlColor);
    }
}

//}}}
//{{{  GetView

CFolditView *CFolditMDIChildWnd::GetView()
{
    return (CFolditView *)GetActiveView();
}

//}}}
//{{{  OnEraseBkGnd

BOOL CFolditMDIChildWnd::OnEraseBkgnd(CDC* pDC)
{
    CMDIChildWnd::OnEraseBkgnd(pDC);

    if (!GetView())
    {
        TRACE0("No View\n");
        return TRUE;
    }
    CBrush* pOldBrush = pDC->SelectObject(&(GetView()->m_BkBrush));

    CRect cliprect;

    pDC->GetClipBox(&cliprect);

    pDC->PatBlt(cliprect.left, cliprect.top, cliprect.Width(), cliprect.Height(), PATCOPY);
    pDC->SelectObject(pOldBrush);

    return TRUE;
}
//}}}

//}}}

//{{{  misc options dlg

void CFolditApp::OnOptionsMiscellaneous()
{
    CMiscOptsDlg dlg;

    dlg.m_DelFoldTails  = App()->m_DelFoldTails;
    dlg.m_Signature     = App()->m_DateTag;
    dlg.m_Restore       = App()->m_MaxPaste;
    dlg.m_SerialNo      = App()->m_SerialNo;
    dlg.m_SerialFmt     = App()->m_SerialFmt;
    dlg.m_EndFoldCue    = App()->m_EndFoldCue;

    dlg.DoModal();

    App()->m_DelFoldTails = dlg.m_DelFoldTails;
    App()->m_DateTag      = dlg.m_Signature;
    App()->m_MaxPaste     = dlg.m_Restore;
    App()->m_SerialNo     = dlg.m_SerialNo;
    App()->m_SerialFmt    = dlg.m_SerialFmt;
    App()->m_EndFoldCue   = dlg.m_EndFoldCue;
}

void CFolditApp::OnUpdateOptionsMiscellaneous(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}

