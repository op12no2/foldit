//{{{}}}
// editlang.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "editlang.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditLangDlg dialog

//{{{  CEditLangDlg::CEditLangDlg(CWnd* pParent /*=NULL*/)
CEditLangDlg::CEditLangDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CEditLangDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CEditLangDlg)
    m_ECom  = "";
    m_Name  = "";
    m_SCom  = "";
    m_Tab   = 0;
    m_FRef  = "";
    m_SFold = "";
    m_EFold = "";
    m_FExt  = "";
    //}}AFX_DATA_INIT
}

//}}}
//{{{  void CEditLangDlg::DoDataExchange(CDataExchange* pDX)
void CEditLangDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEditLangDlg)
    DDX_Control(pDX, IDOK, m_OkButton);
    DDX_Text(pDX, IDC_ECOM, m_ECom);
    DDX_Text(pDX, IDC_NAME, m_Name);
    DDX_Text(pDX, IDC_SCOM, m_SCom);
    DDX_Text(pDX, IDC_SFOLD, m_SFold);
    DDX_Text(pDX, IDC_EFOLD, m_EFold);
    DDX_Text(pDX, IDC_FEXT, m_FExt);
    DDX_Text(pDX, IDC_TAB, m_Tab);
    DDV_MinMaxInt(pDX, m_Tab, 0, 32);
    DDX_Text(pDX, IDC_FREF, m_FRef);
    //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CEditLangDlg, CDialog)
BEGIN_MESSAGE_MAP(CEditLangDlg, CDialog)
    //{{AFX_MSG_MAP(CEditLangDlg)
    ON_EN_CHANGE(IDC_ECOM, OnChangeEcom)
    ON_EN_CHANGE(IDC_NAME, OnChangeName)
    ON_EN_CHANGE(IDC_SCOM, OnChangeScom)
    ON_EN_CHANGE(IDC_SFOLD, OnChangeSFold)
    ON_EN_CHANGE(IDC_EFOLD, OnChangeEFold)
    ON_EN_CHANGE(IDC_TAB, OnChangeTab)
    ON_EN_CHANGE(IDC_FREF, OnChangeFref)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  BOOL CEditLangDlg::OnInitDialog()
BOOL CEditLangDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText ("Edit " + m_Name + " Lang Definition");
    EnableControls();

    return TRUE;
}

//}}}
//{{{  void CEditLangDlg::EnableControls()
void CEditLangDlg::EnableControls()
{
    BOOL Fail = FALSE;

    //Name cannot be NULL
    Fail |= m_Name.IsEmpty();

    m_OkButton.EnableWindow(!Fail);
}

//}}}

/////////////////////////////////////////////////////////////////////////////
// CEditLangDlg message handlers

//{{{  void CEditLangDlg::OnChangeEcom()
void CEditLangDlg::OnChangeEcom()
{
    UpdateData();
    EnableControls();
}

//}}}
//{{{  void CEditLangDlg::OnChangeName()
void CEditLangDlg::OnChangeName()
{
    UpdateData();
    EnableControls();
}

//}}}
//{{{  void CEditLangDlg::OnChangeScom()
void CEditLangDlg::OnChangeScom()
{
    UpdateData();
    EnableControls();
}

//}}}
//{{{  void CEditLangDlg::OnChangeSFold()
void CEditLangDlg::OnChangeSFold()
{
    UpdateData();
    EnableControls();
}

//}}}
//{{{  void CEditLangDlg::OnChangeEFold()
void CEditLangDlg::OnChangeEFold()
{
    UpdateData();
    EnableControls();
}

//}}}
//{{{  void CEditLangDlg::OnChangeTab()
void CEditLangDlg::OnChangeTab()
{
    UpdateData();
    EnableControls();
}

//}}}
//{{{  void CEditLangDlg::OnChangeFref()
void CEditLangDlg::OnChangeFref()
{
    UpdateData();
    EnableControls();
}

//}}}
//{{{  void CEditLangDlg::OnChangeFExt()
void CEditLangDlg::OnChangeFExt()
{
    UpdateData();
    EnableControls();
}

//}}}

