//{{{}}}
// navigati.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNavigationDlg dialog

class CNavigationDlg : public CDialog
{
// Construction
public:
        CNavigationDlg(CWnd* pParent = NULL);  // standard constructor

// Dialog Data
        //{{AFX_DATA(CNavigationDlg)
        enum { IDD = IDD_NAVIGATION };
        BOOL   m_CloseBehaviour;
        BOOL   m_OpenBehaviour;
        BOOL   m_AlwaysRip;
        BOOL   m_MaxViews;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CNavigationDlg)
        afx_msg void OnAlwaysrip();
        afx_msg void OnMaximiseviews();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
