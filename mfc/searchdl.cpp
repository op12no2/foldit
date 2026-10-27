//{{{}}}
// searchdl.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "searchdl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

//{{{  CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CSearchDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CSearchDlg)
        m_SearchString = "";
        //}}AFX_DATA_INIT
}

//}}}
//{{{  void CSearchDlg::DoDataExchange(CDataExchange* pDX)
void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CSearchDlg)
        DDX_Text(pDX, IDC_EDIT1, m_SearchString);
        //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
        //{{AFX_MSG_MAP(CSearchDlg)
               // NOTE: the ClassWizard will add message map macros here
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg message handlers

