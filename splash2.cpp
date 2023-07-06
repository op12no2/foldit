//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd
// splash2.cpp: CSplashWnd class

//{{{  history

// 20/11/99 DCN Use AfxRegisterWndClass not "AfxWnd" when creating the splash window

//}}}

//{{{  includes

#include "stdafx.h"
#include "foldit.h"
#include "resource.h"
#include <dos.h>
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}
//{{{  CSplashWnd

CSplashWnd::CSplashWnd()
{
}

//}}}
//{{{  ~CSPlashWnd

CSplashWnd::~CSplashWnd()
{
    m_bmSplash.DeleteObject();
}

//}}}
//{{{  Create

BOOL CSplashWnd::Create()
{
    m_bmSplash.LoadBitmap(IDB_SPLASHWND);

    BITMAP bm;
    m_bmSplash.GetObject(sizeof(BITMAP), &bm);

    // Get the size of the splash window
    m_wndWidth = bm.bmWidth;
    m_wndHeight = bm.bmHeight;

    // Get the size of the screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // get top/left coord to center the splash window
    int top = (screenHeight - m_wndHeight)/3;
    int left = (screenWidth - m_wndWidth)/2;

    // register the window class
    CString strWndClass = AfxRegisterWndClass(0);

    return CWnd::CreateEx(WS_EX_TOPMOST, strWndClass, "", WS_POPUP | WS_VISIBLE,
        left, top, m_wndWidth, m_wndHeight, NULL, NULL);
}

//}}}
//{{{  MESSAGE_MAP

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
    //{{AFX_MSG_MAP(CSplashWnd)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  OnPaint

void CSplashWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    CBitmap* pOld = memDC.SelectObject(&m_bmSplash);

    if (pOld == NULL)
        return;     // destructors will clean up

    dc.BitBlt(0, 0, m_wndWidth, m_wndHeight, &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOld);

    // Do not call CWnd::OnPaint() for painting messages
}

//}}}

