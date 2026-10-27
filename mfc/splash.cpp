//{{{}}}
// splash.cpp: CSplash class
// splash.cpp: CAboutDlg class
// splash.cpp: CBigIcon class

//{{{  includes
#include "stdafx.h"
#include "foldit.h"
#include "resource.h"
#include <dos.h>
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}
//{{{  CAboutDlg class

//{{{  MESSAGE_MAP

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    ON_BN_CLICKED(IDC_README, OnClickedReadme)
    ON_BN_CLICKED(IDC_COMMENT, OnClickedComment)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  CAboutDlg

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

//}}}
//{{{  DoDataExchange

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

//}}}
//{{{  OnInitDialog

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CString str;
    str.LoadString(AFX_IDS_APP_TITLE);
    SetDlgItemText(IDC_AVAIL_MEM, str);

    CenterWindow();
    return TRUE;  // return TRUE unless you set the focus to a control
}

//}}}
//{{{  OnClickedReadMe

void CAboutDlg::OnClickedReadme()
{
    CString ReadMe ("\\readme.txt");

    App()->OpenDocumentFile (App()->m_ExePath + ReadMe);
    CDialog::OnOK();
}

//}}}
//{{{  OnClickedComment

void CAboutDlg::OnClickedComment()
{
    CString Comment ("\\comment.txt");

    App()->OpenDocumentFile (App()->m_ExePath + Comment);
    CDialog::OnOK();
}
//}}}

//}}}
//{{{  CSplash class

//{{{  CSplash

CSplash::CSplash(CWnd* pParent /*=NULL*/)
    : CDialog(CSplash::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSplash)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

//}}}
//{{{  DoDataExchange

void CSplash::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSplash)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

//}}}
//{{{  MESSAGE_MAP

BEGIN_MESSAGE_MAP(CSplash, CDialog)
    //{{AFX_MSG_MAP(CSplash)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  Create

BOOL CSplash::Create(CWnd* pParent)
{
    //{{AFX_DATA_INIT(CSplashWnd)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    if (!CDialog::Create(CSplash::IDD, pParent))
    {
        TRACE0("Warning: creation of CSplashWnd dialog failed\n");
        return FALSE;
    }

    return TRUE;
}

//}}}
//{{{  OnInitDialog

BOOL CSplash::OnInitDialog()
{
    CDialog::OnInitDialog();

    // initialize the big icon control
    m_icon.SubclassDlgItem(IDC_BIGICON, this);
    m_icon.SizeToContent();

    CenterWindow();
    return TRUE;  // return TRUE  unless you set the focus to a control
}

//}}}

//}}}
//{{{  CBigIconClass

//{{{  MESSAGE_MAP
BEGIN_MESSAGE_MAP(CBigIcon, CButton)
    //{{AFX_MSG_MAP(CBigIcon)
    ON_WM_DRAWITEM()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//}}}
//{{{  constants
#define CY_SHADOW   4
#define CX_SHADOW   4

//}}}
//{{{  SizeToContent
void CBigIcon::SizeToContent()
{
    // get system icon size
    int cxIcon = ::GetSystemMetrics(SM_CXICON);
    int cyIcon = ::GetSystemMetrics(SM_CYICON);

    // a big icon should be twice the size of an icon + shadows
    SetWindowPos(NULL, 0, 0, cxIcon*2 + CX_SHADOW + 4, cyIcon*2 + CY_SHADOW + 4,
        SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
}

//}}}
//{{{  DrawItem
void CBigIcon::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    ASSERT(pDC != NULL);

    CRect rect;
    GetClientRect(rect);
    int cxClient = rect.Width();
    int cyClient = rect.Height();

    // load icon
    HICON hicon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    if (hicon == NULL)
        return;

    // draw icon into off-screen bitmap
    int cxIcon = ::GetSystemMetrics(SM_CXICON);
    int cyIcon = ::GetSystemMetrics(SM_CYICON);

    CBitmap bitmap;
    if (!bitmap.CreateCompatibleBitmap(pDC, cxIcon, cyIcon))
        return;
    CDC dcMem;
    if (!dcMem.CreateCompatibleDC(pDC))
        return;
    CBitmap* pBitmapOld = dcMem.SelectObject(&bitmap);
    if (pBitmapOld == NULL)
        return;

    // blt the bits already on the window onto the off-screen bitmap
    dcMem.StretchBlt(0, 0, cxIcon, cyIcon, pDC,
        2, 2, cxClient-CX_SHADOW-4, cyClient-CY_SHADOW-4, SRCCOPY);

    // draw the icon on the background
    dcMem.DrawIcon(0, 0, hicon);

    // draw border around icon
    CPen pen;
    pen.CreateStockObject(BLACK_PEN);
    CPen* pPenOld = pDC->SelectObject(&pen);
    pDC->Rectangle(0, 0, cxClient-CX_SHADOW, cyClient-CY_SHADOW);
    if (pPenOld)
        pDC->SelectObject(pPenOld);

    // draw shadows around icon
    CBrush br;
    br.CreateStockObject(DKGRAY_BRUSH);
    rect.SetRect(cxClient-CX_SHADOW, CY_SHADOW, cxClient, cyClient);
    pDC->FillRect(rect, &br);
    rect.SetRect(CX_SHADOW, cyClient-CY_SHADOW, cxClient, cyClient);
    pDC->FillRect(rect, &br);

    // draw the icon contents
    pDC->StretchBlt(2, 2, cxClient-CX_SHADOW-4, cyClient-CY_SHADOW-4,
        &dcMem, 0, 0, cxIcon, cyIcon, SRCCOPY);
}

//}}}
//{{{  OnEraseBkGnd
BOOL CBigIcon::OnEraseBkgnd(CDC*)
{
    return TRUE;    // we don't do any erasing...
}

//}}}


//}}}

