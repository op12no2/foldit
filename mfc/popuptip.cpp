//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd
// popuptip.cpp: CPopupTip class

//{{{  history
///////////////////////////////////////////////////////////////////////////
//
// 20/3/96  CRJ Folded.
//
///////////////////////////////////////////////////////////////////////////

//}}}

//{{{  includes

#include "stdafx.h"
#include "popuptip.h"
#include "tooltip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#if defined(STYLE3D)
#define DeflateRect(rect,nCount) \
            rect.left += nCount;   \
            rect.top += nCount;    \
            rect.bottom -= nCount; \
            rect.right -= nCount;
#endif
//}}}
//{{{  MESSAGE_MAP

BEGIN_MESSAGE_MAP(CPopupTip, CWnd)
    //{{AFX_MSG_MAP(CPopupTip)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  CPopupTip

CPopupTip::CPopupTip(CString &strClass)
{
    m_strClass = strClass;
    m_pFont = NULL;
}

//}}}
//{{{  ~CPopupTip

CPopupTip::~CPopupTip()
{
}

//}}}
//{{{  SetDisplay
void CPopupTip::SetDisplay(const CString &strDisplay)
{
    m_strDisplay = strDisplay;
}

//}}}
//{{{  PostNcDestroy
void CPopupTip::PostNcDestroy( )
{
    delete this;
}

//}}}
//{{{  PreCreateWindow
BOOL CPopupTip::PreCreateWindow( CREATESTRUCT& cs )
{
    return TRUE;
}
//}}}
//{{{  PopItUp

BOOL CPopupTip::PopItUp(POINT pt, UINT nNewStyle)
{

#if defined(STYLE3D)
    if (nNewStyle < TTIPS_SQUARESTYLE || nNewStyle > TTIPS_3DSTYLE)
        nNewStyle = TTIPS_SQUARESTYLE;
#else
    if (nNewStyle < TTIPS_SQUARESTYLE || nNewStyle > TTIPS_ROUNDSTYLE)
        nNewStyle = TTIPS_SQUARESTYLE;
#endif

    int  nBorder = 2;

#if defined(STYLE3D)
    if (nNewStyle == TTIPS_3DSTYLE)
        nBorder += 6;
#endif

//
// Get the width and height of the window based on length of text
//
    CWindowDC dc(NULL);
    CFont *pfontOld = dc.SelectObject(m_pFont);
    CString strExt = m_strDisplay + "1";
    CSize sizeText = dc.GetTextExtent(strExt, strExt.GetLength());
    dc.SelectObject(pfontOld);

    sizeText.cx += nBorder,
    sizeText.cy += (sizeText.cy / 4) + nBorder;
//
// See of the box is off the screen to the right.  If it is, move it back
// onto the screen (just barely)
//
    int ScreenRight = GetSystemMetrics(SM_CXSCREEN);
    if (pt.x + sizeText.cx > ScreenRight - 6)
        pt.x = ScreenRight - sizeText.cx - 6;

    if (CreateEx( 0,
        m_strClass,
        NULL,
        WS_POPUP,
        pt.x,
        pt.y,
        sizeText.cx,
        sizeText.cy,
        NULL,
        0) ) {
            ShowWindow(SW_SHOWNOACTIVATE);
            m_nStyleFlag = nNewStyle;
            return TRUE;
    }
    else
      return FALSE;
}

//}}}
//{{{  OnPaint

void CPopupTip::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    CRect rectClient;
    GetClientRect(&rectClient);

#if defined(STYLE3D)
    if (m_nStyleFlag == TTIPS_3DSTYLE)
        Draw3DFrame(&dc, rectClient);
    else {
#endif
//
// Draw the rectangle box for the tip
//
#if defined(USE_APPWORKSPACE_COLORS)
        CBrush brYellow(::GetSysColor(COLOR_APPWORKSPACE));
#else
        CBrush brYellow(RGB(255,255,128));  // Standard tool tip yellow
                                            // take out the 128 for darker
                                            // yellow.
#endif
        CBrush *pBrushOld = dc.SelectObject(&brYellow);

        if( m_nStyleFlag == TTIPS_ROUNDSTYLE ) {
            CPoint ellipsePts(5,10);
            dc.RoundRect( &rectClient, ellipsePts );
        }
        else
            dc.Rectangle( &rectClient );
        dc.SelectObject(pBrushOld);
#if defined(STYLE3D)
    }
#endif
    CFont *pFontOld = NULL;
    if (m_pFont)
        pFontOld = dc.SelectObject(m_pFont);

    //force colors
    int nBkMode = dc.SetBkMode(TRANSPARENT);

#if defined(USE_APPWORKSPACE_COLORS)
    COLORREF crText = dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
#else
    COLORREF crText = dc.SetTextColor(RGB(0,0,0));  // use black for text
#endif

    dc.DrawText( m_strDisplay,
        -1,
        &rectClient,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    dc.SetTextColor(crText);
    dc.SetBkMode(nBkMode);

    if (pFontOld)
        dc.SelectObject(pFontOld);
}


//}}}
//{{{  OnEraseBkgnd
BOOL CPopupTip::OnEraseBkgnd(CDC* pDC)
{
   return TRUE;
}

//}}}
//{{{  Draw3DFrame

#if defined(STYLE3D)

void CPopupTip::Draw3DFrame(CDC* pDC, CRect& rcFrame)
{
//
// Draw a 3D frame for the tip box
//
    CPen    penBlack(PS_SOLID,1,RGB(0, 0, 0));
    CPen    penWhite(PS_SOLID,1,::GetSysColor(COLOR_BTNHIGHLIGHT));
    CPen    penGray(PS_SOLID,1,::GetSysColor(COLOR_BTNFACE));
    CPen    penDark(PS_SOLID,1,::GetSysColor(COLOR_BTNSHADOW));

    CPen*   pOldPen     = (CPen*)   pDC->SelectObject( &penBlack );
    CBrush* pOldBrush   = (CBrush*) pDC->SelectStockObject(LTGRAY_BRUSH);

    DeflateRect(rcFrame,1);            // Frame the window with a black line
    pDC->Rectangle(&rcFrame);
    pDC->MoveTo( rcFrame.left, rcFrame.bottom );
    pDC->LineTo( rcFrame.TopLeft() );
    pDC->LineTo( rcFrame.right, rcFrame.top );
    pDC->LineTo( rcFrame.BottomRight() );
    pDC->LineTo( rcFrame.left, rcFrame.bottom );

    DeflateRect(rcFrame,1);
    pDC->SelectObject( &penWhite );    // Draw white highlight
    pDC->MoveTo( rcFrame.left, rcFrame.bottom );
    pDC->LineTo( rcFrame.TopLeft() );
    pDC->LineTo( rcFrame.right+1, rcFrame.top );

    pDC->SelectObject( &penDark );    // Draw the dark shadow
    pDC->MoveTo( rcFrame.right, rcFrame.top+1 );
    pDC->LineTo( rcFrame.BottomRight() );
    pDC->LineTo( rcFrame.left, rcFrame.bottom );

    DeflateRect(rcFrame,1);
    pDC->SelectObject( &penGray );    // Draw the gray frame
    pDC->MoveTo( rcFrame.left, rcFrame.bottom );
    pDC->LineTo( rcFrame.TopLeft() );
    pDC->LineTo( rcFrame.right, rcFrame.top );
    pDC->LineTo( rcFrame.BottomRight() );
    pDC->LineTo( rcFrame.left, rcFrame.bottom );

    DeflateRect(rcFrame,1);
    pDC->SelectObject( &penDark );  // Draw the inside dark shadow
    pDC->MoveTo( rcFrame.left, rcFrame.bottom );
    pDC->LineTo( rcFrame.TopLeft() );
    pDC->LineTo( rcFrame.right, rcFrame.top );

    pDC->SelectObject( &penWhite ); // Draw the inside white hightlight
    pDC->MoveTo( rcFrame.right, rcFrame.top+1 );
    pDC->LineTo( rcFrame.BottomRight() );
    pDC->LineTo( rcFrame.left, rcFrame.bottom );

    ////////////////////////////////////////////////////////
    // Be good and restore the DC to as it was            //
    ////////////////////////////////////////////////////////
    pDC->SelectObject( (CPen*) pOldPen );
    pDC->SelectObject( (CBrush*) pOldBrush);
    // Now clean up the temp GDI objects
    penWhite.DeleteObject();
    penGray.DeleteObject();
    penDark.DeleteObject();
    penBlack.DeleteObject();
}

#endif
//}}}

