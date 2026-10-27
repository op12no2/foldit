//{{{}}}
// foldpres.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFoldPresentationDlg dialog

class CFoldPresentationDlg : public CDialog
{
// Construction
public:
        CFoldPresentationDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
        //{{AFX_DATA(CFoldPresentationDlg)
        enum { IDD = IDD_FOLDPRESENTATION };
        CString        m_FoldPrefix;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CFoldPresentationDlg)
               // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
