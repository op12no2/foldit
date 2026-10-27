//{{{}}}
// pastelis.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasteListDlg dialog

class CPasteListDlg : public CDialog
{
// Construction
public:
        CPasteListDlg(CFoldLine *pPasteList, CFoldLine **pPasteBuffer);        // standard constructor
        BOOL OnInitDialog();
// Dialog Data
        CFoldLine *m_pPasteList;
        CFoldLine **m_ppPasteBuffer;
        //{{AFX_DATA(CPasteListDlg)
        enum { IDD = IDD_PASTELIST };
        CEdit  m_PasteView;
        CListBox       m_PasteList;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CPasteListDlg)
        virtual void OnOK();
        afx_msg void OnSelchangePastelist();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
