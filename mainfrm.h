//{{{}}}
// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "tooltip.h"

//{{{  CBarFind class
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBarFind window

class CBarFind : public CComboBox
{
// Construction
public:
    CBarFind();

// Attributes
public:

// Operations
public:

// Implementation
public:
    virtual ~CBarFind();

protected:
    // Generated message map functions
    //{{AFX_MSG(CBarFind)
        afx_msg void OnSelEndOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//}}}
//{{{  CMainFrame class

class CMainFrame : public CMDIFrameWnd
{
        DECLARE_DYNAMIC(CMainFrame)
public:
        CMainFrame();

// Attributes
public:

// Operations
public:

// Implementation
public:
        virtual ~CMainFrame();
#ifdef _DEBUG
        virtual        void AssertValid() const;
        virtual        void Dump(CDumpContext& dc) const;
#endif
        void UpdateFindEnv();
        void SetToolbarOptions(BOOL);
protected:
        BOOL OnCommand(UINT wParam, LONG lParam);

public:        // control bar embedded members
        CStatusBar         m_wndStatusBar;
        CToolTipBar        m_wndToolBar;
        CBarFind           m_wndFindBox;
        CWnd*              m_pFocus;

// Generated message map functions
protected:
        //{{AFX_MSG(CMainFrame)
        afx_msg void OnClose();
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnOptionsLanguages();
        afx_msg void OnUpdateOptionsLanguages(CCmdUI* pCmdUI);
        afx_msg void OnOptionsNavigation();
        afx_msg void OnUpdateOptionsNavigation(CCmdUI* pCmdUI);
        afx_msg void OnOptionsFoldpresentation();
        afx_msg void OnUpdateOptionsFoldpresentation(CCmdUI* pCmdUI);
        afx_msg void OnToolbar();
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnWindowFindstring();
    afx_msg void OnUpdateWindowFindstring(CCmdUI* pCmdUI);
    //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

//}}}

