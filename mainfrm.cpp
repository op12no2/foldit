// mainfrm.cpp: CMainFrame class

//{{{  includes

#include "stdafx.h"
#include "foldit.h"

#include "mainfrm.h"
#include "foldidoc.h"
#include "langlst.h"
#include "sellang.h"
#include "foldpres.h"
#include "navigati.h"
#include "toolopti.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}
//{{{  MESSAGE_MAP

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CLOSE()
    ON_WM_CREATE()
    ON_COMMAND(ID_OPTIONS_LANGUAGES, OnOptionsLanguages)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGES, OnUpdateOptionsLanguages)
    ON_COMMAND(ID_OPTIONS_NAVIGATION, OnOptionsNavigation)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_NAVIGATION, OnUpdateOptionsNavigation)
    ON_COMMAND(ID_OPTIONS_FOLDPRESENTATION, OnOptionsFoldpresentation)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_FOLDPRESENTATION, OnUpdateOptionsFoldpresentation)
    ON_COMMAND(IDD_TOOLBAR, OnToolbar)
    ON_WM_KEYDOWN()
    ON_WM_CTLCOLOR()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_COMMAND(ID_WINDOW_FINDSTRING, OnWindowFindstring)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_FINDSTRING, OnUpdateWindowFindstring)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  toolbar buttons

static UINT BASED_CODE buttons[] =
{
    // same order as in the bitmap 'toolbar.bmp'

    ID_FILE_PRINT,
        ID_SEPARATOR,
    ID_FILE_NEW,
    ID_FILE_OPEN,
    ID_FILE_SAVE,
        ID_SEPARATOR,
        ID_SEPARATOR,        //combo here
        ID_SEPARATOR,
    ID_EDIT_FINDFIRST,
    ID_EDIT_FINDNEXT,
        ID_SEPARATOR,
    ID_EDIT_CUT,
    ID_EDIT_COPY,
    ID_EDIT_PASTE,
        ID_SEPARATOR,
    ID_TOOLS_INSERTDATE,
    ID_TOOLS_OUTDENTALLLINES,
    ID_TOOLS_INDENTALLLINES,
    ID_EDIT_RESTORE,
        ID_SEPARATOR,
    ID_APP_ABOUT,
    ID_CONTEXT_HELP,
};

//}}}
//{{{  status bar indicators

static UINT BASED_CODE indicators[] =
{
    ID_SEPARATOR,
    ID_INDICATOR_DIRTY,
    ID_INDICATOR_LAST,
    ID_INDICATOR_WHO,
    ID_INDICATOR_RED,
    ID_INDICATOR_LINE,
    ID_INDICATOR_COL,
    ID_INDICATOR_LANG
};
//}}}
//{{{  CMainFrame

CMainFrame::CMainFrame()
{
    m_pFocus=NULL;
}

//}}}
//{{{  ~CMainFrame

CMainFrame::~CMainFrame()
{
}

//}}}

//{{{  OnClose

void CMainFrame::OnClose()
{
  WINDOWPLACEMENT place;
  place.length=sizeof(place);

  if (GetWindowPlacement(&place))
  {
    CIniFile::WriteInt ("Defaults", "MainWinLeft",   place.rcNormalPosition.left);
    CIniFile::WriteInt ("Defaults", "MainWinRight",  place.rcNormalPosition.right);
    CIniFile::WriteInt ("Defaults", "MainWinTop",    place.rcNormalPosition.top);
    CIniFile::WriteInt ("Defaults", "MainWinBottom", place.rcNormalPosition.bottom);
    CIniFile::WriteUInt("Defaults", "MainWinShow",   place.showCmd);
  }
  else
    AfxMessageBox("failed to save window placement");

  m_wndFindBox.GetWindowText(App()->m_SearchStr);
  CMDIFrameWnd::OnClose();
}

//}}}
//{{{  OnCreate

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    int Width;
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.Create(this) ||
        !m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
        !m_wndToolBar.SetButtons(buttons,
          sizeof(buttons)/sizeof(UINT)))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;        // fail to create
    }

    SetToolbarOptions(FALSE);

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
          sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;        // fail to create
    }

    Width=250;
    CIniFile::ReadInt ("Defaults","FindWidth",Width);
    m_wndToolBar.SetButtonInfo(6, ID_BAR_FIND, TBBS_SEPARATOR, Width);
    RECT rect;
    m_wndToolBar.GetItemRect(6,&rect);
    rect.bottom+=Width;
    if (!m_wndFindBox.Create(CBS_NOINTEGRALHEIGHT|CBS_AUTOHSCROLL|CBS_DROPDOWN|WS_VSCROLL|WS_HSCROLL|WS_VISIBLE|WS_TABSTOP,rect,&m_wndToolBar,ID_BAR_FIND))
    {
        TRACE0("Failed to create find combo on toolbat\n");
        return -1;
    }
    return 0;
}

//}}}
//{{{  OnCommand

BOOL CMainFrame::OnCommand(UINT wParam, LONG lParam)
{
    //if (COleClientItem::InWaitForRelease())
    //{
    //    AfxMessageBox(IDP_BUSY);
    //    return TRUE;        // handled
    //}
    return CMDIFrameWnd::OnCommand(wParam, lParam);
}

//}}}
//{{{  debugging

#ifdef _DEBUG

void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif

//}}}
//{{{  OnOptionsLanguages

void CMainFrame::OnOptionsLanguages()
{
    CSelLangDlg dlg;

    dlg.DoModal();
}

//}}}
//{{{  OnUpdateOptionsLanguages

void CMainFrame::OnUpdateOptionsLanguages(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  OnOptionsNavigation

void CMainFrame::OnOptionsNavigation()
{
    CNavigationDlg dlg;

    dlg.m_OpenBehaviour     = App()->m_NavEnterRefP;
    dlg.m_CloseBehaviour    = App()->m_NavAutoCloseP;
    dlg.m_AlwaysRip         = App()->m_AlwaysRip;
    dlg.m_MaxViews          = App()->m_MaxViews;

    dlg.DoModal();

    App()->m_NavEnterRefP  = dlg.m_OpenBehaviour;
    App()->m_NavAutoCloseP = dlg.m_CloseBehaviour;
    App()->m_AlwaysRip     = dlg.m_AlwaysRip;
    App()->m_MaxViews      = dlg.m_MaxViews;
}

//}}}
//{{{  OnUpdateOptionsNavigation

void CMainFrame::OnUpdateOptionsNavigation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  OnOptionsFoldpresentation

void CMainFrame::OnOptionsFoldpresentation()
{
    CFoldPresentationDlg dlg;

    dlg.m_FoldPrefix = CFoldLine::m_Prefix;

    dlg.DoModal();

    CFoldLine::m_Prefix      = dlg.m_FoldPrefix;
}

//}}}
//{{{  OnUpdateOptionsFoldpresentation

void CMainFrame::OnUpdateOptionsFoldpresentation(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  SetToolbarOptions

void CMainFrame::SetToolbarOptions(BOOL bDisplayDialog)
{
    CToolOptions ToolDlg;
    int iStatus;

    ToolDlg.m_StatusLine = m_wndToolBar.GetFlyby();
    ToolDlg.m_Popup = m_wndToolBar.GetTips();
    ToolDlg.m_BoxStyle = m_wndToolBar.GetTipStyle();
    ToolDlg.m_SystemFont = m_wndToolBar.GetTipFont();
    ToolDlg.m_Wait = m_wndToolBar.GetTipWait();
    ToolDlg.m_ToolBar = m_wndToolBar.GetToolbarDisplay();

    if (bDisplayDialog)
        iStatus = ToolDlg.DoModal ();
    else
        iStatus = IDOK;

    if (iStatus == IDOK) {
        m_wndToolBar.SetFlyby(ToolDlg.m_StatusLine);
        m_wndToolBar.SetTips(ToolDlg.m_Popup);
        m_wndToolBar.SetTipStyle(ToolDlg.m_BoxStyle);
        m_wndToolBar.SetTipFont(ToolDlg.m_SystemFont);
        m_wndToolBar.SetTipWait(ToolDlg.m_Wait);
        m_wndToolBar.SetToolbarDisplay(ToolDlg.m_ToolBar);
        RecalcLayout();
    }
}

//}}}
//{{{  OnToolBar

void CMainFrame::OnToolbar()
{
    SetToolbarOptions(TRUE);
}

//}}}
//{{{  OnKeyDown

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    MessageBeep(0xFFFFFFFF);
    if (!GetActiveDocument())
        AfxMessageBox ("no docs");

    CMDIFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//}}}
//{{{  OnCtlColor

HBRUSH CMainFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    TRACE0 ("ctl col in main frame\n");

    return CMDIFrameWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

//}}}
//{{{  OnSetFocus

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
    CMDIFrameWnd::OnSetFocus(pOldWnd);

    // TODO: Add your message handler code here

}

//}}}
//{{{  OnKillFocus

void CMainFrame::OnKillFocus(CWnd* pNewWnd)
{
    CMDIFrameWnd::OnKillFocus(pNewWnd);
}
//}}}
//{{{  UpdateFindEnv

void CMainFrame::UpdateFindEnv()
{
  CString CurFind;

  m_wndFindBox.GetWindowText(CurFind);

  if (!CurFind.IsEmpty())
    if (m_wndFindBox.FindString(-1,CurFind) == CB_ERR)
      m_wndFindBox.AddString(CurFind);
}
//}}}
//{{{  focus control
void CMainFrame::OnWindowFindstring()
{
  if (!m_pFocus)
  {
    m_pFocus = m_wndFindBox.SetFocus();
  }
  else
  {
    //((CEditView *)GetActiveView())->GetEditCtrl().SetFocus();
    m_pFocus->SetFocus();
    m_pFocus = NULL;
  }

}

void CMainFrame::OnUpdateWindowFindstring(CCmdUI* pCmdUI)
{
  if (!m_pFocus)
  {
    pCmdUI->SetText("Find Box\tF11");
    pCmdUI->Enable(TRUE);
  }
  else
  {
    pCmdUI->SetText("Edit Window\tF11");
    pCmdUI->Enable(TRUE);
  }
}
//}}}
//{{{  CBarFind
/////////////////////////////////////////////////////////////////////////////
// CBarFind

CBarFind::CBarFind()
{
}

CBarFind::~CBarFind()
{
}

BEGIN_MESSAGE_MAP(CBarFind, CComboBox)
    //{{AFX_MSG_MAP(CBarFind)
    ON_CBN_SELENDOK(ID_BAR_FIND,OnSelEndOK)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarFind message handlers

void CBarFind::OnSelEndOK()
{
   AfxMessageBox("got it in cbar");
}

//}}}

