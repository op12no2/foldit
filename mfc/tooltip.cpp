//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd

#include "stdafx.h"
#include "tooltip.h"
#include "popuptip.h"
#include "resource.h"
#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CToolTipBar, CToolBar)

#define new DEBUG_NEW

//{{{  MESSAGE_MAP

BEGIN_MESSAGE_MAP(CToolTipBar, CToolBar)
    //{{AFX_MSG_MAP(CToolTipBar)
    ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_CBN_SELENDOK(ID_BAR_FIND,OnSelEndOK)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  void CToolTipBar::OnSelEndOK()
void CToolTipBar::OnSelEndOK()
{
   AfxMessageBox("got it in parent");
}


//}}}
//{{{  CToolTipBar::CToolTipBar()
CToolTipBar::CToolTipBar()
{
    m_bDirty = FALSE;  // haven't done a tip window
    m_dwTicks = 0;     // haven't moved over toolbar
    m_uIdCache = 0;       // no previous toolbar button
    m_pPopup = NULL;   // no tip window displayed (or created)
    m_strPopupClassName = ::AfxRegisterWndClass(CS_SAVEBITS,
                          ::AfxGetApp()->LoadStandardCursor(IDC_ARROW));

    m_pFont = new CFont;
    if (m_pFont)
       SetupFont();

    GetToolbarOptions();
    m_bMousePressed = FALSE;
}

//}}}
//{{{  CToolTipBar::~CToolTipBar()
CToolTipBar::~CToolTipBar()
{
    WriteToolbarOptions();
    if (m_pFont)
       delete m_pFont;
    m_pIDXCustom.RemoveAll();
}

//}}}
//{{{  void CToolTipBar::SetupFont()
void CToolTipBar::SetupFont()
{
    CFont*   pTipFont;
    LOGFONT  lf;

    if (!m_pFont)
        return;

    pTipFont = CFont::FromHandle((HFONT)::GetStockObject(SYSTEM_FONT));
    pTipFont->GetObject(sizeof(LOGFONT), &lf);

    lf.lfHeight -= 2;
    lf.lfWidth = 0;
    lf.lfWeight = FW_NORMAL;

    if (!m_bSystemFont)
       lstrcpy(lf.lfFaceName,"MS Sans Serif");

    m_pFont->DeleteObject();
    if (!m_pFont->CreateFontIndirect(&lf)) {
       delete m_pFont;
       m_pFont = NULL;
    }
}

//}}}
//{{{  void CToolTipBar::AddCustomButton(UINT nIDXControl)
void CToolTipBar::AddCustomButton(UINT nIDXControl)
{
    m_pIDXCustom.Add(nIDXControl);
}

//}}}
//{{{  BOOL CToolTipBar::DoIdle(LONG lCount)
BOOL CToolTipBar::DoIdle(LONG lCount)
{
    CWnd* pMain = AfxGetApp()->m_pMainWnd;

// If our window is disabled, iconic, we have disable tips, or the mouse
// is pressed, or it not our specific instance ... get out of here ...
    //if (!pMain->IsWindowEnabled() ||
    //    pMain->IsIconic() ||
    //   !IsWindowVisible() ||
    //   !(m_bFlyby | m_bTips) || m_bMousePressed ||
    //   !(AfxGetInstanceHandle() ==
    //   (HINSTANCE)::GetWindowWord(::GetActiveWindow(),GWW_HINSTANCE)) ) {
    //   CleanDirty(TRUE);
    //   return FALSE;
    //}
//
// Get the location of the mouse, see if it is over my toolbar window
//
    POINT ptCur;
    GetCursorPos(&ptCur);

    RECT rcSelf;
    GetWindowRect(&rcSelf);
    if (!PtInRect(&rcSelf, ptCur)) {
       CleanDirty(TRUE);
       return FALSE;
    }
//
// This test makes sure that we do not popup tool tips over the top of any
// non-modal windows which are overlapping our current button.  Basically,
// if the mouse is pointing at a window that is not ours, we're gonna
// get lost ... the actual test is done later after the uId for a custom
// dialog item is found (if there is one), but we have to do this test
// before doing ScreenToClient
//
    BOOL bNotOurWindow = FALSE;
    HWND hMouseWindow = ::WindowFromPoint(ptCur);
    ScreenToClient(&ptCur);
//
// First check to see if we're in any
// customer control area, such as a COMBOBOX
// We have stored the index into the toolbar of the separator where
// a control is (I.E., a COMBOBOX).  With the index we can get the command
// that the box represents.
//
    BOOL bCustom = FALSE;
    int  nIdx = -1;   // 20/11/99 DCN HACK not initialised before

    if  (m_pIDXCustom.GetSize() > 0) {
       // Test each custom button
       CRect rect;
        for (int i = 0 ; i < m_pIDXCustom.GetSize(); ++i) {
            GetItemRect(m_pIDXCustom[i], &rect);  // Get button's rect
            if (rect.PtInRect(ptCur))  {         // It this where we are?
                nIdx = m_pIDXCustom[i];           // then use it's ID
                bCustom = TRUE;                  // and tell the world we got one.
                break;
            }
        }
    }
//
// If it is not a custom area and not our window then we don't want to
// display a tip, even if the mouse is on top of our window.  There is
// probably someone else (a non-modal dialog perhaps) on top of us.
//
    //if (!bCustom)
    //   nIdx =  HitTest(ptCur);

    //TRACE1("\nHitest found index: %d",nIdx);
    if (nIdx == -1) {
       CleanDirty(FALSE);    // Don't blank out the cache cause we're
       if (m_uIdCache > 0) // still on the toolbar (just not on a button)
         m_uIdCache = 0xFFFFFFFF;// In case they go back to the previous button
       return FALSE;     // not over a button
    }

    UINT uId;
    UINT uStyle;
    int nImage;

    GetButtonInfo(nIdx, uId, uStyle, nImage);
//
// If we think we're doing a custom control but we're not in our window
//    or
// we think we're doing a button and we're not in the button's window ...
//
    if ((bCustom &&
        (hMouseWindow != GetDlgItem(uId)->GetSafeHwnd())) ||
        (!bCustom && (hMouseWindow != GetSafeHwnd()))) {
        CleanDirty(TRUE);
       return FALSE;
    }
//
// If we're on the same button then we don't need to do this again
//
    if (uId == m_uIdCache) {
       m_dwTicks = 0;
       return FALSE;
    }
    DWORD dwNow = GetTickCount();

    //did we just move over a different button
    if (m_uIdCache && (uId != m_uIdCache)) {
       CleanDirty(TRUE);
       dwNow = m_nWait + m_dwTicks; // no sense in waiting again
    }

    if (m_dwTicks == 0)
       m_dwTicks = dwNow;
    //
    // See if enough time has passed or if we are just doing flybys and
    // not popups then do it immediately ...
    //
    else if (!m_bTips || (dwNow - m_dwTicks) > m_nWait) {
       // it's time to display the tip
       if (m_bFlyby) {
         pMain->PostMessage(WM_SETMESSAGESTRING, uId, (LPARAM)NULL);
       }
       m_uIdCache = uId;
       m_dwTicks = 0;
       m_bDirty = TRUE;

       if (m_pPopup)
         m_pPopup->PostMessage(WM_CLOSE);
       m_pPopup = NULL;

       if (!m_bTips)
         return FALSE;

       char szTipHold[32];  //it's just a tip

       //try to get the menu name from parent window's menu
       CMenu *pMenu = GetParent()->GetMenu();

       if (pMenu
         && (pMenu->GetMenuString(uId, szTipHold, sizeof(szTipHold),
          MF_BYCOMMAND))) {
         //put a null at the end just in case it was truncated
         szTipHold[sizeof(szTipHold) -1] = '\0';

         //some menu items have ...'s after them, it doesn't look good
         char* pszTab = strstr(szTipHold, "...");
         if (pszTab)
          *pszTab = '\0';

            //look for a tab replace it with a null if found
         pszTab = strchr(szTipHold, '\t');
         if (pszTab)
          *pszTab = '\0';

            //what about & ??
         char *ptr1 = szTipHold;
         char *ptr2 = szTipHold;
         while (*ptr1 != '\0') {
          if(*ptr1 != '&') {
              *ptr2 = *ptr1;
              ++ptr2;
          }
          ++ptr1;
         }
         *ptr2 = '\0';
       }
       // Since there was no menu item for this button we will attempt to
       // get a string table entry for it.  If one is there then the way
       // this works is it looks for the tip somewhere in the line in ()s
       // e.g., Change to (Bold) font   ... display Bold as the tip
       // otherwise it just grabs the first word
       else {
         char szTipWork[64]; // temp string buffer

         //TRACE1("\nAttempt to LoadString(%d)",uId);
         if (::LoadString(::AfxGetInstanceHandle(),
              uId,
              szTipWork, // These messages may be longer
              sizeof(szTipWork)) ) {

          //TRACE1("\nString Loaded (%s)",szTipWork);
          char *ptr1, *ptr2;
          if ((ptr1 = strchr (szTipWork,'(')) != NULL) {
              ++ptr1;
              if ((ptr2 = strchr (szTipWork, ')')) != NULL)
                 *ptr2 = '\0';
          }
          else {
              ptr1 = szTipWork;
              if ((ptr2 = strchr (szTipWork, ' ')) != NULL)
                 *ptr2 = '\0';
          }
          strncpy (szTipHold, ptr1, sizeof(szTipHold) - 1);
          szTipHold[sizeof(szTipHold) -1] = '\0';
         }
            else
          strcpy(szTipHold,"Not Defined");
       }

       m_pPopup = new CPopupTip (m_strPopupClassName);
       if (!m_pPopup) // can't popup window
         return TRUE;

       m_pPopup->SetDisplay(szTipHold);
       m_pPopup->SetFont(m_pFont);

       GetItemRect(nIdx, &rcSelf);
       ClientToScreen(&rcSelf);

       // this will display the popup window
       m_pPopup->PopItUp(GetBoxPosition(rcSelf), m_nStyleFlag);
    }
    return TRUE;
}

//}}}
//{{{  LRESULT CToolTipBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
LRESULT CToolTipBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
    if (DoIdle(0))
       PostMessage(NULL);  // pump myself a message - not ready to sleep
   return CToolBar::OnIdleUpdateCmdUI(wParam, lParam);
}

//}}}
//{{{  void CToolTipBar::SetTipWait (UINT nNewWait = TTIPS_DEFAULT_TIME)
void CToolTipBar::SetTipWait (UINT nNewWait = TTIPS_DEFAULT_TIME)
{
    m_nWait = nNewWait;
}

//}}}
//{{{  void CToolTipBar::SetToolbarDisplay (BOOL bNewToolbarDisplay = TRUE)
void CToolTipBar::SetToolbarDisplay (BOOL bNewToolbarDisplay = TRUE)
{
    m_bToolbarDisplay = bNewToolbarDisplay;
    ShowWindow( m_bToolbarDisplay ? SW_SHOW : SW_HIDE );
}

//}}}
//{{{  void CToolTipBar::SetFlyby (BOOL bNewFlyby = TRUE)
void CToolTipBar::SetFlyby (BOOL bNewFlyby = TRUE)
{
    m_bFlyby = bNewFlyby;
}

//}}}
//{{{  void CToolTipBar::SetTips (BOOL bNewTips = TRUE)
void CToolTipBar::SetTips (BOOL bNewTips = TRUE)
{
    m_bTips = bNewTips;
}

//}}}
//{{{  void CToolTipBar::SetTipStyle (UINT nNewStyle = TTIPS_SQUARESTYLE)
void CToolTipBar::SetTipStyle (UINT nNewStyle = TTIPS_SQUARESTYLE)
{
    m_nStyleFlag = nNewStyle;
}

//}}}
//{{{  void CToolTipBar::SetTipFont (BOOL bNewSystemFont = TRUE)
void CToolTipBar::SetTipFont (BOOL bNewSystemFont = TRUE)
{
    m_bSystemFont = bNewSystemFont;
    SetupFont();
}

//}}}
//{{{  UINT CToolTipBar::GetTipWait ()
UINT CToolTipBar::GetTipWait ()
{
    return m_nWait;
}

//}}}
//{{{  BOOL CToolTipBar::GetFlyby ()
BOOL CToolTipBar::GetFlyby ()
{
    return m_bFlyby;
}

//}}}
//{{{  BOOL CToolTipBar::GetTips ()
BOOL CToolTipBar::GetTips ()
{
    return m_bTips;
}

//}}}
//{{{  UINT CToolTipBar::GetTipStyle ()
UINT CToolTipBar::GetTipStyle ()
{
    return m_nStyleFlag;
}

//}}}
//{{{  BOOL CToolTipBar::GetTipFont ()
BOOL CToolTipBar::GetTipFont ()
{
    return m_bSystemFont;
}

//}}}
//{{{  BOOL CToolTipBar::GetToolbarDisplay()
BOOL CToolTipBar::GetToolbarDisplay()
{
    return m_bToolbarDisplay;
}

//}}}
//{{{  void CToolTipBar::OnLButtonDown(UINT nFlags, CPoint point)
void CToolTipBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    CleanDirty(TRUE);
    m_bMousePressed = TRUE;
    CToolBar::OnLButtonDown(nFlags, point);
}

//}}}
//{{{  void CToolTipBar::OnLButtonUp(UINT nFlags, CPoint point)
void CToolTipBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_bMousePressed = FALSE;
    CToolBar::OnLButtonUp(nFlags, point);
}

//}}}
//{{{  void CToolTipBar::CleanDirty(int ClearCache)
void CToolTipBar::CleanDirty(int ClearCache)
{
    if (m_bDirty) {
       AfxGetApp()->m_pMainWnd->PostMessage(WM_SETMESSAGESTRING,
         AFX_IDS_IDLEMESSAGE, (LPARAM)NULL);
       m_bDirty = FALSE;
       if (m_pPopup) {
         m_pPopup->PostMessage(WM_CLOSE);
         m_pPopup = NULL;
       }
    }
    if (ClearCache)
       m_uIdCache = 0;
    m_dwTicks = 0;
}

//}}}
//{{{  void CToolTipBar::GetToolbarOptions()
void CToolTipBar::GetToolbarOptions()
{
//
// Get current tip information from the app.ini file (if it is there)
//
    CWinApp *pApp = AfxGetApp();

    m_bFlyby = pApp->GetProfileInt(SZTIPINI_SECTION, SZTIPINI_FLYBY, 1);
    m_bTips = pApp->GetProfileInt(SZTIPINI_SECTION, SZTIPINI_TIPS, 1);
    m_nStyleFlag = pApp->GetProfileInt(SZTIPINI_SECTION, SZTIPINI_ROUND, TTIPS_SQUARESTYLE);
    m_nWait = pApp->GetProfileInt(SZTIPINI_SECTION, SZTIPINI_WAIT, TTIPS_DEFAULT_TIME);
    m_bToolbarDisplay = pApp->GetProfileInt(SZTIPINI_SECTION, SZTIPINI_DISPLAY, 1);
    m_bSystemFont = pApp->GetProfileInt(SZTIPINI_SECTION, SZTIPINI_FONT, 0);
}

//}}}
//{{{  void CToolTipBar::WriteToolbarOptions()
void CToolTipBar::WriteToolbarOptions()
{
//
// Write the current settings back to the app.ini file
//
    CWinApp *pApp = AfxGetApp();

    pApp->WriteProfileInt(SZTIPINI_SECTION, SZTIPINI_FLYBY, m_bFlyby ? 1:0);
    pApp->WriteProfileInt(SZTIPINI_SECTION, SZTIPINI_TIPS, m_bTips ? 1:0);
    pApp->WriteProfileInt(SZTIPINI_SECTION, SZTIPINI_ROUND, m_nStyleFlag);
    pApp->WriteProfileInt(SZTIPINI_SECTION, SZTIPINI_WAIT, m_nWait);
    pApp->WriteProfileInt(SZTIPINI_SECTION, SZTIPINI_DISPLAY,
        m_bToolbarDisplay ? 1:0);
    pApp->WriteProfileInt(SZTIPINI_SECTION, SZTIPINI_FONT,
        m_bSystemFont ? 1:0);
}

//}}}
//{{{  POINT CToolTipBar::GetBoxPosition(CRect rcSelf)
POINT CToolTipBar::GetBoxPosition(CRect rcSelf)
{
//
// Cursor pointer bitmap structure ...
// The memory location contains this structure first then the actual bitmap
//
    struct CURSOR_INFO {
       POINT pntHotSpot;
       WORD nWidth;
       WORD nHeight;
       BYTE byPlanes;
       BYTE byBitsPix;
    } CursorInfo;

    POINT pBoxPosition, ptCur;
    GetCursorPos(&pBoxPosition);
    ptCur = pBoxPosition;

    HCURSOR hcur = GetCursor();

    void far *pcurmem = LockResource ((HGLOBAL) hcur);
    if(pcurmem == NULL)
       return pBoxPosition; // Couldn't lock resource ...

    long far *prows = (long far*) pcurmem + 3;
    CursorInfo = *(struct CURSOR_INFO far*) pcurmem;
    UINT i = 0;
//
// There are 12 bytes in the header info and the bitmap follows.  We
// are looking up from the bottom to see where to put the tooltip box.
// 1 bit == 1 pixel so
// 4 bytes == 1 row of 32 pixels
// That's why I use a long for each row
//
    for (i=CursorInfo.nHeight - 1; i >= 0; i--) {
       if (*(prows+i) != 0xffffffff)
         break; // found some bits eh ...
    }
    pBoxPosition.y += (i + 1);
//
// Need to check to see if the box is going to be off the screen on the
// bottom.  Then move the box to the top of the toolbar button if it is
// not going to fit on the screen otherwise.
//
    int ScreenBottom = GetSystemMetrics(SM_CYSCREEN);
    if ((pBoxPosition.y + CursorInfo.nHeight) > (WORD)ScreenBottom) {

       LOGFONT lf;
       m_pFont->GetObject(sizeof(LOGFONT),&lf);

       pBoxPosition = rcSelf.TopLeft();
       pBoxPosition.x = ptCur.x;

       // back off size of text plus 5 for frame and just to space it a bit
       pBoxPosition.y -= (lf.lfHeight + 5);
       if (m_nStyleFlag == TTIPS_3DSTYLE)
         pBoxPosition.y -= 5;
       if (m_bSystemFont == TRUE)
         pBoxPosition.y -= 2; // This font is just slightly larger
    }
    UnlockResource ((HGLOBAL) hcur);
    return pBoxPosition;
}

//}}}

