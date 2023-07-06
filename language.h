//{{{}}}
// language.h : header file
//

#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

///////////////////////////////////////////////////////////
//CLanguage

//CLanguage embodies information about each source language available

class CLanguage : public CObject
{
public:
        CString m_Name;     //language name
        CString m_SCom;     //start comment for folds in files
        CString m_ECom;     //end comment for folds in files
        CString m_SFold;    //start fold mark for folds in files
        CString m_EFold;    //end fold mark for folds in files
        int     m_Tab;      //tab width
        CString m_Ref;      //file reference prefix
        CString m_FExt;     //file extension (used to set language when no markers present)
        BOOL    m_ReadOnly;

        CLanguage::CLanguage(CLanguage *pLang = NULL);
};

#endif
