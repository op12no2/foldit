//{{{}}}
// sellang.cpp : implementation file
//

#include "stdafx.h"
#include "foldit.h"
#include "foldidoc.h"
#include "langlst.h"
#include "sellang.h"
#include "editlang.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelLangDlg dialog

//{{{  CSelLangDlg::CSelLangDlg(CFolditDoc *pDoc)
CSelLangDlg::CSelLangDlg(CFolditDoc *pDoc)
        : CDialog(CSelLangDlg::IDD, NULL)
{
        m_pDoc = pDoc;
        m_pLanguage = NULL;

        //{{AFX_DATA_INIT(CSelLangDlg)
               // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
}

//}}}
//{{{  void CSelLangDlg::DoDataExchange(CDataExchange* pDX)
void CSelLangDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CSelLangDlg)
        DDX_Control(pDX, IDCANCEL, m_CancelButton);
        DDX_Control(pDX, IDC_LNEW, m_NewButton);
        DDX_Control(pDX, IDC_LINISAVE, m_IniSaveButton);
        DDX_Control(pDX, IDC_LINILOAD, m_IniLoadButton);
        DDX_Control(pDX, IDC_LEDIT, m_EditButton);
        DDX_Control(pDX, IDC_LDEL, m_DelButton);
        DDX_Control(pDX, IDOK, m_OkButton);
        DDX_Control(pDX, IDC_LANGUAGELIST, m_Languages);
        //}}AFX_DATA_MAP
}

//}}}
//{{{  BEGIN_MESSAGE_MAP(CSelLangDlg, CDialog)
BEGIN_MESSAGE_MAP(CSelLangDlg, CDialog)
        //{{AFX_MSG_MAP(CSelLangDlg)
        ON_LBN_SELCANCEL(IDC_LANGUAGELIST, OnSelcancelLanguagelist)
        ON_BN_CLICKED(IDC_LDEL, OnClickedLdel)
        ON_BN_CLICKED(IDC_LEDIT, OnClickedLedit)
        ON_BN_CLICKED(IDC_LINILOAD, OnClickedLiniload)
        ON_BN_CLICKED(IDC_LINISAVE, OnClickedLinisave)
        ON_BN_CLICKED(IDC_LNEW, OnClickedLnew)
        ON_LBN_DBLCLK(IDC_LANGUAGELIST, OnDblclkLanguagelist)
        ON_LBN_SETFOCUS(IDC_LANGUAGELIST, OnSetfocusLanguagelist)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//}}}
//{{{  BOOL CSelLangDlg::OnInitDialog()
BOOL CSelLangDlg::OnInitDialog()
{
        CDialog::OnInitDialog(); //Always do first

        if (m_pDoc)
               SetWindowText ("Select Language");
        else
               SetWindowText ("Edit Language Definitions");

        FillLangBox();
        EnableControls();
        m_Languages.SetCurSel(m_Languages.GetCount() - 1);
        UpdateData(FALSE);

        return TRUE; //Tell windows to do first focus
}

//}}}
//{{{  void CSelLangDlg::EnableControls()
void CSelLangDlg::EnableControls()
{
        BOOL IsLang = (BOOL)m_Languages.GetCount();

        m_OkButton.EnableWindow(IsLang);
        m_DelButton.EnableWindow(IsLang);
        m_EditButton.EnableWindow(IsLang);
}

//}}}
//{{{  void CSelLangDlg::FillLangBox ()
void CSelLangDlg::FillLangBox ()
{
        int idx;
        CString Next;

        m_Languages.ResetContent();

        for (int Lang = 0; Lang < App()->m_pLanguages->GetSize(); Lang++)
        {
               CLanguage *pNextLang = (CLanguage *)(App()->m_pLanguages->GetAt(Lang));

               Next = pNextLang->m_Name;

               idx = m_Languages.AddString(Next);
               m_Languages.SetItemDataPtr (idx, pNextLang);
        }

        if (m_pDoc)
        {
             //Select lang currently defined in document if it still exists
             for (idx = 0; idx < m_Languages.GetCount(); idx++)
                 if (m_pDoc->m_pLanguage == m_Languages.GetItemDataPtr(idx))
                 {
                    m_Languages.SetCurSel (idx);
                       return;
                   }
    }

        m_Languages.SetCurSel(0);
}

//}}}

/////////////////////////////////////////////////////////////////////////////
// CSelLangDlg message handlers

//{{{  void CSelLangDlg::OnSelcancelLanguagelist()
void CSelLangDlg::OnSelcancelLanguagelist()
{
        ASSERT(TRUE);
}

//}}}
//{{{  void CSelLangDlg::OnClickedLiniload()
void CSelLangDlg::OnClickedLiniload()
{
        App()->m_pLanguages->ReadFromIniFile();
        FillLangBox();
        EnableControls();
        UpdateData(FALSE);
}

//}}}
//{{{  void CSelLangDlg::OnClickedLinisave()
void CSelLangDlg::OnClickedLinisave()
{
        App()->m_pLanguages->WriteToIniFile();
}

//}}}
//{{{  void CSelLangDlg::OnDblclkLanguagelist()
void CSelLangDlg::OnDblclkLanguagelist()
{
        OnClickedLedit();
}

//}}}
//{{{  void CSelLangDlg::OnOK()
void CSelLangDlg::OnOK()
{
        //Assume at least one style exists at this point
        //OK disabled otherwise

        m_pLanguage = (CLanguage *)m_Languages.GetItemDataPtr(m_Languages.GetCurSel());

        if (m_pDoc)
        {
               m_pDoc->m_pLanguage = m_pLanguage;
        }

        else
        {
               ;
        }

        EndDialog(IDOK);
}

//}}}
//{{{  void CSelLangDlg::OnClickedLdel()
void CSelLangDlg::OnClickedLdel()
{
    CLanguage *pLang = (CLanguage *)(m_Languages.GetItemDataPtr(m_Languages.GetCurSel()));
    ASSERT (pLang);

    if (pLang->m_ReadOnly)
    {
        AfxMessageBox ("This language definition cannot be deleted.",
                       MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
        return;
    }

    if (m_pDoc)
    {
        CMultiDocTemplate *pTemplate = (CMultiDocTemplate *)m_pDoc->GetDocTemplate();
        ASSERT (pTemplate);

        POSITION pos = pTemplate->GetFirstDocPosition();
        while (pos)
        {
           CFolditDoc *pDoc = (CFolditDoc *)pTemplate->GetNextDoc (pos);
           if (pLang == pDoc->m_pLanguage)
           {
             AfxMessageBox ("This Language is presently being\n"
                            "used by at least one open file.\n"
                            "\nIt cannot be deleted.",
                            MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
             return;
           }
        }
    }

    VERIFY(((CFolditApp *)AfxGetApp())->m_pLanguages->Delete (pLang));

        FillLangBox();
        EnableControls();
        UpdateData(FALSE);
}

//}}}
//{{{  void CSelLangDlg::OnClickedLedit()
void CSelLangDlg::OnClickedLedit()
{
        CLanguage *pLang = (CLanguage *)m_Languages.GetItemDataPtr (m_Languages.GetCurSel());
    ASSERT (pLang);

        if (pLang->m_ReadOnly)
    {
        AfxMessageBox ("This Language definition cannot be edited.",
                       MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
        return;
    }

        CEditLangDlg dlg;

        dlg.m_SCom  = pLang->m_SCom;
        dlg.m_ECom  = pLang->m_ECom;
        dlg.m_SFold = pLang->m_SFold;
        dlg.m_EFold = pLang->m_EFold;
        dlg.m_Name  = pLang->m_Name;
        dlg.m_Tab   = pLang->m_Tab;
        dlg.m_FRef  = pLang->m_Ref;
        dlg.m_FExt  = pLang->m_FExt;

        if (dlg.DoModal() == IDOK)
               m_CancelButton.SetWindowText("Done");

        pLang->m_SCom  = dlg.m_SCom;
        pLang->m_ECom  = dlg.m_ECom;
        pLang->m_SFold = dlg.m_SFold;
        pLang->m_EFold = dlg.m_EFold;
        pLang->m_Name  = dlg.m_Name;
        pLang->m_Tab   = dlg.m_Tab;
        pLang->m_Ref   = dlg.m_FRef;
        pLang->m_FExt  = dlg.m_FExt;

        FillLangBox();

        //reselect existing language
        //(not doc language)
        for (int i = 0; i < m_Languages.GetCount(); i++)
               if (m_Languages.GetItemDataPtr(i) == pLang)
                     m_Languages.SetCurSel(i);

    EnableControls();
    UpdateData(FALSE);
}

//}}}
//{{{  void CSelLangDlg::OnClickedLnew()
void CSelLangDlg::OnClickedLnew()
{

        CEditLangDlg dlg;

        if (m_Languages.GetCount())
        {
               CLanguage *pLang = (CLanguage *)m_Languages.GetItemDataPtr (m_Languages.GetCurSel());

               dlg.m_SCom  = pLang->m_SCom;
               dlg.m_ECom  = pLang->m_ECom;
               dlg.m_SFold = pLang->m_SFold;
               dlg.m_EFold = pLang->m_EFold;
               dlg.m_Tab   = pLang->m_Tab;
               dlg.m_FRef  = pLang->m_Ref;
               dlg.m_FExt  = pLang->m_FExt;
        }

        if (dlg.DoModal() == IDOK)
        {

        CLanguage *pNewLang = ((CFolditApp *)AfxGetApp())->m_pLanguages->New();

               pNewLang->m_SCom  = dlg.m_SCom;
               pNewLang->m_ECom  = dlg.m_ECom;
               pNewLang->m_SFold = dlg.m_SFold;
               pNewLang->m_EFold = dlg.m_EFold;
               pNewLang->m_Name  = dlg.m_Name;
               pNewLang->m_Tab   = dlg.m_Tab;
               pNewLang->m_Ref   = dlg.m_FRef;
               pNewLang->m_FExt  = dlg.m_FExt;

               FillLangBox();
    }

    EnableControls();
    UpdateData(FALSE);
}

//}}}
//{{{  void CSelLangDlg::OnSetfocusLanguagelist()
void CSelLangDlg::OnSetfocusLanguagelist()
{
        // TODO: Add your control notification handler code here
}

//}}}

