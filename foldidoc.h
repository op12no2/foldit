//{{{}}}
// foldidoc.h : interface of the CFolditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FOLDIDOC_H__
#define __FOLDIDOC_H__

#ifndef __TEXTLINE_H__
#include "textline.h"
#endif

#ifndef __FOLDLINE_H__
#include "foldline.h"
#endif

#ifndef __INIFILE_H__
#include "inifile.h"
#endif

class CFolditDoc : public CDocument
{
protected: // create from serialization only
        CFolditDoc();
        DECLARE_DYNCREATE(CFolditDoc)

// Attributes
public:

public: //data
        CLanguage  *m_pLanguage;
        CFoldLine   m_TopFold;
        BOOL        m_Sticky;
        CFileStatus m_FileStatus;
        CString     m_Folder;            //full path name to the folder the file is in
        CString     m_Extension;         //extension of that file (for possible language selection)

// Operations
public:

// Implementation
public:
        CFoldLine *GetTop(){return &m_TopFold;};
        virtual ~CFolditDoc();
        virtual void Serialize(CArchive& ar);  // overridden for document i/o
        virtual        void DeleteContents();
        virtual int GetViewCount();

#ifdef _DEBUG
        virtual        void AssertValid() const;
        virtual        void Dump(CDumpContext& dc) const;
#endif

protected:

    virtual void InitDoc();
    virtual     BOOL OnNewDocument();
        virtual BOOL OnSaveDocument (const char *pszPathName);
        virtual BOOL OnOpenDocument (const char *pszPathName);

// Generated message map functions
public:
        //{{AFX_MSG(CFolditDoc)
        afx_msg void OnOptionsLanguages();
        afx_msg void OnUpdateOptionsLanguages(CCmdUI* pCmdUI);
        afx_msg void OnUpdateIndicatorLang(CCmdUI* pCmdUI);
        afx_msg void OnFileClose();
        afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif

