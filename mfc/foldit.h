//{{{}}}
// foldit.h : main header file for the FOLDIT application
//

#define __FOLDIT_H__

#ifndef __AFXWIN_H__
        #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"           // main symbols

#ifndef __LANGLST_H__
#include "langlst.h"
#endif

#ifndef __FOLDIDOC_H__
#include "foldidoc.h"
#endif

#ifndef __FOLDIVW_H__
#include "foldivw.h"
#endif


#include "splash.h"
#include "splash2.h"

/////////////////////////////////////////////////////////////////////////////
// CFolditApp:
// See foldit.cpp for the implementation of this class
//

//{{{  class CFolditApp : public CWinApp
class CFolditApp : public CWinApp
{
public:
    CView           *m_PendingView;
    char            m_ProfileStr[128];
    CUIntArray      m_KeyMacroChar;
    CUIntArray      m_KeyMacroFunc;
    CUIntArray      m_KeyMacroFlags;
    CUIntArray      m_KeyMacroRep;
    BOOL            m_KeyMacroActive;
    BOOL            m_KeyMacroPlayBack;
    BOOL            m_GotTime;
    CStringArray    m_FolditFileTypes;
    CStringArray    m_Assoc;
    CStringArray    m_AssocExt;
    CSplashWnd      *m_pwndSplash;
    DWORD           m_dwSplashTime;
    CFoldLine       *m_pPasteFold;
    CLanguageList   *m_pLanguages;
    CLanguage       *m_pDefaultLanguage;
    CString         m_DefaultFontName;
    int             m_DefaultFontHeight;
    UINT            m_SerialNo;
    CString         m_SerialFmt;
    CString         m_FileFilter;
    CStringArray    m_RefPrefixes;
    CStringArray    m_RefPostfixes;
    CStringArray    m_Red;
    BOOL            m_NavEnterRefP;
    BOOL            m_NavAutoCloseP;
    CString         m_ExePath;
    const char      *m_pSavedProfileName;
    OPENFILENAME    m_ofn;
    CFolditDoc      *m_pPasteDoc;
    CFoldLine       m_PasteStack;
    int             m_MaxPaste;
    int             m_AlwaysRip;
    COLORREF        m_DefBkGnd;
    COLORREF        m_DefFrGnd;
    CString         m_SearchStr;
    BOOL            m_MaxViews;
    CString         m_DateTag;
    BOOL            m_DelFoldTails;
    CString         m_InitFind;
    CFoldLine       *m_pLastFind;
    CString         m_EndFoldCue;

public:
        CFolditApp();

// Overrides
        virtual BOOL InitInstance();
        int ExitInstance();
    BOOL OnIdle (LONG lCount);
        virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
    BOOL DoPromptFileName(CString& fileName,
                              DWORD lFlags,
                                  BOOL bOpenFileDialog,
                                  CDocTemplate *pTemplate);

    int Exec (CString File);
    CFoldLine *NewPaste(CString Title);
    void CopyToClipboard    (CLanguage *pLang);
    int  PasteFromClipboard (CLanguage *pLang);

        //{{AFX_MSG(CFolditApp)
        afx_msg void OnAppAbout();
        afx_msg void OnFileOpen();
        afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
        afx_msg void OnHelpTutorial();
        afx_msg void OnEditUndo();
        afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
        afx_msg void OnEditRestore();
        afx_msg void OnUpdateEditRestore(CCmdUI* pCmdUI);
    afx_msg void OnOptionsMiscellaneous();
    afx_msg void OnUpdateOptionsMiscellaneous(CCmdUI* pCmdUI);
    //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

//}}}

#ifndef __HELPER_H__
#include "helper.h"
#endif

#ifndef __SENTENCE_H__
#include "sentence.h"
#endif

/////////////////////////////////////////////////////////////////////////////

#define U_ERR (-1)
#define U_FILEBUFFER   2048
#define U_UPDATEBUFFER (8192-1)
#define U_LANGSEARCHBUFFER 512
#define U_NUMERIC_PAD_MASK 0x100


/////////////////////////////////////////////////////////////////////////////
// CFolditMDIChildWnd frame

//{{{  class CFolditMDIChildWnd : public CMDIChildWnd
class CFolditMDIChildWnd : public CMDIChildWnd
{
        DECLARE_DYNCREATE(CFolditMDIChildWnd)
protected:
        CFolditMDIChildWnd();                        // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
        virtual ~CFolditMDIChildWnd();
        virtual BOOL PreCreateWindow (CREATESTRUCT &cs);
        CFolditView *GetView();

        // Generated message map functions
        //{{AFX_MSG(CFolditMDIChildWnd)
        afx_msg void OnKillFocus(CWnd* pNewWnd);
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

//}}}

/////////////////////////////////////////////////////////////////////////////
