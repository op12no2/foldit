//{{{}}}
// editstyl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditLangDlg dialog

class CEditLangDlg : public CDialog
{
// Construction
public:
        CEditLangDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
        //{{AFX_DATA(CEditLangDlg)
        enum { IDD = IDD_EDITLANG };
        CButton        m_OkButton;
        CString        m_ECom;
        CString        m_Name;
        CString        m_SCom;
        CString        m_SFold;
        CString        m_EFold;
        int            m_Tab;
        CString        m_FRef;
        CString        m_FExt;
        //}}AFX_DATA

// Implementation
protected:
        BOOL OnInitDialog();
        void EnableControls();
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CEditLangDlg)
        afx_msg void OnChangeEcom();
        afx_msg void OnChangeName();
        afx_msg void OnChangeScom();
        afx_msg void OnChangeSFold();
        afx_msg void OnChangeEFold();
        afx_msg void OnChangeTab();
        afx_msg void OnChangeFref();
        afx_msg void OnChangeFExt();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
