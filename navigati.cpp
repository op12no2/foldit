//{{{}}}
// navigati.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "navigati.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavigationDlg dialog

//{{{  CNavigationDlg::CNavigationDlg(CWnd* pParent /*=NULL*/)
CNavigationDlg::CNavigationDlg(CWnd* pParent /*=NULL*/)
        : CDialog(CNavigationDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CNavigationDlg)
        m_CloseBehaviour = FALSE;
        m_OpenBehaviour = FALSE;
        m_AlwaysRip = FALSE;
        m_MaxViews = FALSE;
        //}}AFX_DATA_INIT
}

//}}}
//{{{  void CNavigationDlg::DoDataExchange(CDataExchange* pDX)
void CNavigationDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CNavigationDlg)
        DDX_Check(pDX, IDC_NAVFOLDCLOSE, m_CloseBehaviour);
        DDX_Check(pDX, IDC_NAVFOLDOPEN, m_OpenBehaviour);
        DDX_Check(pDX, IDC_ALWAYSRIP, m_AlwaysRip);
        DDX_Check(pDX, IDC_MAXIMISEVIEWS, m_MaxViews);
        //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CNavigationDlg, CDialog)
BEGIN_MESSAGE_MAP(CNavigationDlg, CDialog)
        //{{AFX_MSG_MAP(CNavigationDlg)
        ON_BN_CLICKED(IDC_ALWAYSRIP, OnAlwaysrip)
        ON_BN_CLICKED(IDC_MAXIMISEVIEWS, OnMaximiseviews)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}

/////////////////////////////////////////////////////////////////////////////
// CNavigationDlg message handlers

//{{{  void CNavigationDlg::OnAlwaysrip()
void CNavigationDlg::OnAlwaysrip()
{
    UpdateData(TRUE);
        if (m_AlwaysRip)
          m_MaxViews = FALSE;
        UpdateData(FALSE);
}

//}}}
//{{{  void CNavigationDlg::OnMaximiseviews()
void CNavigationDlg::OnMaximiseviews()
{
   UpdateData(TRUE);
   if (m_MaxViews)
          m_AlwaysRip = FALSE;
   UpdateData(FALSE);

}

//}}}

