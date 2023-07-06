//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd
// popuptip.h : header file
//

#ifndef     _POPUPTIP
#define     _POPUPTIP

// Comment the next line to leave out 3D stuff (saves a tiny bit of memory)
#define STYLE3D

// UnComment the next line to use the system colors rather than yellow/black
// for the popup tips window

//#define USE_APPWORKSPACE_COLORS

/////////////////////////////////////////////////////////////////////////////
// CPopupTip frame

class CPopupTip : public CWnd
{

public:
        CPopupTip(CString &strClass);
        virtual ~CPopupTip();
        BOOL PopItUp (POINT pt, UINT bNewStyle);
        UINT m_nStyleFlag;

// Attributes
public:
        CFont *m_pFont;
        CString m_strDisplay;
        CString m_strClass;

// Operations
public:
        virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
        void SetDisplay(const CString &strDisplay);
        void SetFont(CFont *pFont) {m_pFont = pFont; };

// Implementation
protected:
        virtual void PostNcDestroy( );
#if defined(STYLE3D)
    void Draw3DFrame(CDC* pDC, CRect& rcFrame);
#endif
        // Generated message map functions
        //{{AFX_MSG(CpopupWnd)
        afx_msg void OnPaint();
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
#endif

