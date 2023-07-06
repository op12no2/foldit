//{{{}}}
// sellang.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelLangDlg dialog

class CSelLangDlg : public CDialog
{
// Construction
public:
        CSelLangDlg(CFolditDoc *pDoc = NULL);  // standard constructor

// Dialog Data
        CFolditDoc *m_pDoc;
        CLanguage  *m_pLanguage;

        //{{AFX_DATA(CSelLangDlg)
        enum { IDD = IDD_SELLANG };
        CButton        m_CancelButton;
        CButton        m_NewButton;
        CButton        m_IniSaveButton;
        CButton        m_IniLoadButton;
        CButton        m_EditButton;
        CButton        m_DelButton;
        CButton        m_OkButton;
        CListBox       m_Languages;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support
    void EnableControls();
    BOOL OnInitDialog();
    void FillLangBox();

        // Generated message map functions
        //{{AFX_MSG(CSelLangDlg)
        afx_msg void OnSelcancelLanguagelist();
        afx_msg void OnClickedLdel();
        afx_msg void OnClickedLedit();
        afx_msg void OnClickedLiniload();
        afx_msg void OnClickedLinisave();
        afx_msg void OnClickedLnew();
        afx_msg void OnDblclkLanguagelist();
        virtual void OnOK();
        afx_msg void OnSetfocusLanguagelist();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
