//{{{}}}
// miscopts.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "miscopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiscOptsDlg dialog


//{{{  CMiscOptsDlg::CMiscOptsDlg(CWnd* pParent /*=NULL*/)
CMiscOptsDlg::CMiscOptsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CMiscOptsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CMiscOptsDlg)
    m_DelFoldTails = FALSE;
    m_Restore = 0;
    m_SerialFmt = "";
    m_Signature = "";
    m_EndFoldCue = "";
        m_SerialNo = 0;
        //}}AFX_DATA_INIT
}

//}}}
//{{{  void CMiscOptsDlg::DoDataExchange(CDataExchange* pDX)
void CMiscOptsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMiscOptsDlg)
    DDX_Check(pDX, IDC_DELFOLDTAILS, m_DelFoldTails);
    DDX_Text(pDX, IDC_RESTORE, m_Restore);
    DDV_MinMaxInt(pDX, m_Restore, 2, 100);
    DDX_Text(pDX, IDC_SERIALFMT, m_SerialFmt);
    DDX_Text(pDX, IDC_SIGNATURE, m_Signature);
    DDV_MaxChars(pDX, m_Signature, 3);
    DDX_Text(pDX, IDC_ENDFOLDCUE, m_EndFoldCue);
        DDX_Text(pDX, IDC_SERIALNO, m_SerialNo);
        //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CMiscOptsDlg, CDialog)
BEGIN_MESSAGE_MAP(CMiscOptsDlg, CDialog)
    //{{AFX_MSG_MAP(CMiscOptsDlg)
       // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}

/////////////////////////////////////////////////////////////////////////////
// CMiscOptsDlg message handlers

