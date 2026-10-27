//{{{}}}
// toolopti.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolOptions dialog

class CToolOptions : public CDialog
{
// Construction
public:
        CToolOptions(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
        //{{AFX_DATA(CToolOptions)
        enum { IDD = IDD_TOOLBAR };
        BOOL   m_Popup;
        BOOL   m_StatusLine;
        BOOL   m_ToolBar;
        UINT   m_Wait;
        int       m_BoxStyle;
        BOOL   m_SystemFont;
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support
        virtual BOOL OnInitDialog();
        // Generated message map functions
        //{{AFX_MSG(CToolOptions)
               // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};
