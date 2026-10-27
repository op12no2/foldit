//{{{}}}
// toolopti.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "toolopti.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolOptions dialog


//{{{  CToolOptions::CToolOptions(CWnd* pParent /*=NULL*/)
CToolOptions::CToolOptions(CWnd* pParent /*=NULL*/)
        : CDialog(CToolOptions::IDD, pParent)
{
        //{{AFX_DATA_INIT(CToolOptions)
        m_BoxStyle = -1;
        m_SystemFont = FALSE;
        //}}AFX_DATA_INIT
}

//}}}
//{{{  void CToolOptions::DoDataExchange(CDataExchange* pDX)
void CToolOptions::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CToolOptions)
        DDX_Check(pDX, IDC_POPUP, m_Popup);
        DDX_Check(pDX, IDC_STATUSLINE, m_StatusLine);
        DDX_Check(pDX, IDC_TOOLBAR, m_ToolBar);
        DDX_Text(pDX, IDC_WAITTIME, m_Wait);
        DDV_MinMaxUInt(pDX, m_Wait, 10, 32000);
        DDX_Radio(pDX, IDC_SQUARE, m_BoxStyle);
        DDX_Check(pDX, IDC_SYSTEMFONT, m_SystemFont);
        //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CToolOptions, CDialog)
BEGIN_MESSAGE_MAP(CToolOptions, CDialog)
        //{{AFX_MSG_MAP(CToolOptions)
               // NOTE: the ClassWizard will add message map macros here
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//}}}

/////////////////////////////////////////////////////////////////////////////
// CToolOptions message handlers

//{{{  BOOL CToolOptions::OnInitDialog()
BOOL CToolOptions::OnInitDialog()
{
        CenterWindow();
        UpdateData(FALSE);
        return TRUE;
}

//}}}

