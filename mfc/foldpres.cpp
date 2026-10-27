//{{{}}}
// foldpres.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "foldpres.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFoldPresentationDlg dialog

//{{{  CFoldPresentationDlg::CFoldPresentationDlg(CWnd* pParent /*=NULL*/)
CFoldPresentationDlg::CFoldPresentationDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CFoldPresentationDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CFoldPresentationDlg)
        m_FoldPrefix = "";
        //}}AFX_DATA_INIT
}

//}}}
//{{{  void CFoldPresentationDlg::DoDataExchange(CDataExchange* pDX)
void CFoldPresentationDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CFoldPresentationDlg)
        DDX_Text(pDX, IDC_FOLDPRESPREFIX, m_FoldPrefix);
        //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CFoldPresentationDlg, CDialog)
BEGIN_MESSAGE_MAP(CFoldPresentationDlg, CDialog)
        //{{AFX_MSG_MAP(CFoldPresentationDlg)
               // NOTE: the ClassWizard will add message map macros here
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}

/////////////////////////////////////////////////////////////////////////////
// CFoldPresentationDlg message handlers

