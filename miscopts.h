//{{{}}}
// miscopts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMiscOptsDlg dialog

class CMiscOptsDlg : public CDialog
{
// Construction
public:
    CMiscOptsDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    //{{AFX_DATA(CMiscOptsDlg)
        enum { IDD = IDD_MISCOPTS };
    BOOL       m_DelFoldTails;
    int        m_Restore;
    CString    m_SerialFmt;
    CString    m_Signature;
    CString    m_EndFoldCue;
        UINT   m_SerialNo;
        //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CMiscOptsDlg)
       // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
