//{{{}}}
// langlst.h : header file
//

#ifndef __LANGUAGE_H__
#include "language.h"
#endif

#ifndef __LANGLST_H__
#define __LANGLST_H__

///////////////////////////////////////////////////////////
// CLanguageList

//CLanguageList is an array of CLanguage objects.

class CLanguageList : public CObArray
{
public:
        CLanguageList::CLanguageList();
        CLanguageList::~CLanguageList();

        void ReadFromIniFile ();
        void WriteToIniFile();
        CLanguage *GetLanguageFromName (const char *Name);
        CLanguage *New();
        BOOL Delete(CLanguage *);
        CLanguage *FindLanguage (CString Buf, CString Ext);
};

#endif
