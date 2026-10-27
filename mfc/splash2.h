//{{{}}}

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd window

#define SPLASH_TIME 1000

class CSplashWnd : public CWnd
{
// Construction
public:
    CSplashWnd();

// Attributes
public:
    virtual BOOL Create();

// Operations
public:

// Implementation
public:
    virtual ~CSplashWnd();

protected:
    CBitmap m_bmSplash;
    int m_wndWidth;
    int m_wndHeight;

    // Generated message map functions
    //{{AFX_MSG(CSplashWnd)
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

