//{{{}}}
// language.cpp
//

#include "stdafx.h"
#include "langlst.h"
#include "foldit.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//CLanguage

//{{{  CLanguage::CLanguage(CLanguage *pLang)
//copy constructor

CLanguage::CLanguage(CLanguage *pLang)
{
        if (pLang)
        {
               m_Name        = pLang->m_Name;
               m_SCom        = pLang->m_SCom;
               m_ECom        = pLang->m_ECom;
               m_SFold       = pLang->m_SFold;
               m_EFold       = pLang->m_EFold;
               m_Tab         = pLang->m_Tab;
               m_Ref         = pLang->m_Ref;
               m_FExt        = pLang->m_FExt;
        }

        else
        {
               m_Name        = "";
               m_SCom        = "";
               m_ECom        = "";
               m_SFold       = "";
               m_EFold       = "";
               m_Tab         = 0;
               m_Ref         = "";
               m_FExt        = "";
        }

        m_ReadOnly = FALSE;
}

//}}}

