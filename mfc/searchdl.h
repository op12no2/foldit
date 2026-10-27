//{{{}}}
// searchdl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

class CSearchDlg : public CDialog
{
// Construction
public:
        CSearchDlg(CWnd* pParent = NULL);      // standard constructor

// Dialog Data
        //{{AFX_DATA(CSearchDlg)
        enum { IDD = IDD_SEARCH };
        CString        m_SearchString;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CSearchDlg)
               // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
