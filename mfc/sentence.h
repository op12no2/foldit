//{{{}}}

#ifndef __SENTENCE_H__

#define __SENTENCE_H__

class CFoldLine;

class CSentence : public CString
{
public:
        CSentence (){};
        CSentence (const CString& stringSrc) : CString (stringSrc) {};
        CSentence (const char* psz) : CString (psz) {};
        CSentence (char ch, int nRepeat = 1) : CString (ch, nRepeat) {};
        CSentence (const char* pch, int nLength) : CString (pch, nLength) {};
        //CSentence (const char FAR* lpsz) : CString (lpsz) {};
        //CSentence (const char FAR* lpch, int nLength) : CString (lpch, nLength) {};
        CSentence (const CSentence& stringSrc) : CString ((CString)stringSrc) {};
        ~CSentence();

public: //Operations
    void ReadFoldsTo (CFoldLine *pFold, CLanguage *pDocLang);
    int StripLine   (char *Buf, int *Next);
        void ExpandTabs (const int TabSize = 4, const char TabChar = (char)9, const char RepChar = ' ');
        int GetIndent (const char WhiteSpaceChar = ' ') const;
        BOOL ExtractBetween (const char Delim, CString &Extract);
        BOOL ExtractBetween (const char Delim1, const char Delim2, CString &Extract);
        int FindTrailingWhite ();
        void RemoveTrailingWhite ();
};

#endif
