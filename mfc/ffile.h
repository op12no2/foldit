//{{{}}}

#ifndef __FFILE__
#define __FFILE__

#ifndef __LANGLST__
#include "langlst.h"
#endif

class CFoldedFile : public  CStdioFile
{
public:
        CLanguage *m_pLanguage;

public:
    CLanguage *SetLanguage (CLanguage *pLang = NULL, CString Ext = "");
    void InitLang () {m_pLanguage = NULL;};

    CFoldedFile (const char *pszFileName, UINT nOpenFlags) : CStdioFile (pszFileName, nOpenFlags) {InitLang();};
    CFoldedFile () : CStdioFile () {InitLang();};
    CFoldedFile (FILE *pOpenStream) : CStdioFile (pOpenStream) {InitLang();};

        void ReadFoldsTo    (CFoldLine *pFold);
        void WriteFoldsFrom (CFoldLine *pFold);
        void WriteFold      (CFoldLine *pFold, int CurIndent);
};

#endif
