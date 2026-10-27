//{{{}}}
// pastelis.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "pastelis.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasteListDlg dialog

//{{{  CPasteListDlg::CPasteListDlg(CFoldLine *pPasteList, CFoldLine **ppPasteBuffer)
CPasteListDlg::CPasteListDlg(CFoldLine *pPasteList, CFoldLine **ppPasteBuffer)
        : CDialog(CPasteListDlg::IDD, NULL)
{
        m_pPasteList = pPasteList;
        m_ppPasteBuffer = ppPasteBuffer;

        //{{AFX_DATA_INIT(CPasteListDlg)
               // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
}

//}}}
//{{{  void CPasteListDlg::DoDataExchange(CDataExchange* pDX)
void CPasteListDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CPasteListDlg)
        DDX_Control(pDX, IDC_PASTEVIEW, m_PasteView);
        DDX_Control(pDX, IDC_PASTELIST, m_PasteList);
        //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CPasteListDlg, CDialog)
BEGIN_MESSAGE_MAP(CPasteListDlg, CDialog)
        //{{AFX_MSG_MAP(CPasteListDlg)
        ON_LBN_SELCHANGE(IDC_PASTELIST, OnSelchangePastelist)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}

/////////////////////////////////////////////////////////////////////////////
// CPasteListDlg message handlers

//{{{  void CPasteListDlg::OnOK()
void CPasteListDlg::OnOK()
{
        int Index = m_PasteList.GetCurSel();
        CFoldLine *pPaste = (CFoldLine *)m_PasteList.GetItemDataPtr(Index);

        *m_ppPasteBuffer = pPaste;

        //POSITION pos = m_pPasteList->m_LineList.FindIndex(Index);
        //m_pPasteList->m_LineList.RemoveAt(pos);

        CDialog::OnOK();
}

//}}}
//{{{  BOOL CPasteListDlg::OnInitDialog()
BOOL CPasteListDlg::OnInitDialog()
{
        CString Title;
        POSITION pos = m_pPasteList->m_LineList.GetHeadPosition();

        BOOL ret = CDialog::OnInitDialog();

    int Index=0;

        while (pos)
        {
            CFoldLine *pFold = (CFoldLine *)m_pPasteList->m_LineList.GetNext(pos);
            CLine *pLine = pFold->GetLineAt(0);

            Title = pFold->m_Body; //time etc

            if (pLine->IsFoldP())
               Title += CFoldLine::m_Prefix;

            Title += pLine->m_Body;

            m_PasteList.InsertString(0, Title);
            m_PasteList.SetItemDataPtr(0, (void *)pFold);
            Index++;
        }

        m_PasteList.SetCurSel(0);
        OnSelchangePastelist();
        return ret;
}

//}}}
//{{{  void CPasteListDlg::OnSelchangePastelist()
void CPasteListDlg::OnSelchangePastelist()
{
    CSentence Buf;
    CFoldLine *pFold = (CFoldLine *)m_PasteList.GetItemDataPtr(m_PasteList.GetCurSel());
    pFold->WriteFoldsToString(pFold, Buf, App()->m_pDefaultLanguage);
    m_PasteView.SetWindowText(Buf);
}

//}}}

