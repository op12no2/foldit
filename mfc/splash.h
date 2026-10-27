//{{{}}}
// splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBigIcon window

class CBigIcon : public CButton
{
// Attributes
public:

// Operations
public:
        void SizeToContent();

// Implementation
protected:
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

        //{{AFX_MSG(CBigIcon)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
        CAboutDlg();

// Dialog Data
        //{{AFX_DATA(CAboutDlg)
        enum { IDD = IDD_ABOUTBOX };
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support
        CBigIcon m_icon; // self-draw button
    CBigIcon m_icon2;

        // Generated message map functions
        //{{AFX_MSG(CAboutDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnClickedReadme();
        afx_msg void OnClickedComment();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSplash dialog

class CSplash : public CDialog
{
// Construction
public:
        CSplash(CWnd* pParent = NULL); // standard constructor
        BOOL Create(CWnd* pParent);

// Dialog Data
        //{{AFX_DATA(CSplash)
        enum { IDD = IDD_SPLASH };
               // NOTE: the ClassWizard will add data members here
        //}}AFX_DATA

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support
        CBigIcon m_icon; // self-draw button
        CFont m_font;   // light version of dialog font

        // Generated message map functions
        //{{AFX_MSG(CSplash)
        virtual BOOL OnInitDialog();
               // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

