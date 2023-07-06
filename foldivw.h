//{{{}}}
// foldivw.h : interface of the CFolditView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FOLDIVW_H__
#define __FOLDIVW_H__

#ifndef __TEXTLINE_H__
#include "textline.h"
#endif

#ifndef __FOLDLINE_H__
#include "foldline.h"
#endif

//the blind hint is also the default used by MFC on initial update
#define HINT_BLIND              0
#define HINT_REPLACE    1

class CHintBlind : public CObject
{
public:

    CFoldLine       *m_pCurrent;
    DWORD           m_Sel;
    int             m_TopLine;
};

class CHintReplace : public CHintBlind
{
public:
    CString m_Chars;
    int             m_StartSel;
    int             m_EndSel;
};

class CFolditView : public CEditView
{
protected: // create from serialization only
    CFolditView();
    DECLARE_DYNCREATE(CFolditView)

// Attributes
public:

    CFolditDoc*     m_pDoc;         //pointer to parent doc
    CFoldLine*      m_pCurrent;     //pointer to current fold in doc
    CFont*          m_pFont;        //current font
    CHOOSEFONT      m_cf;           //results of CFontDialog
    LOGFONT         m_lf;           //more results of CFontDialog
    CHintBlind      m_DefaultHint;  //default blind hint
    CObList         m_OnExitFifo;   //used to save position of caret, scroll pos used
                                    //to restore on exit
    CBrush          m_BkBrush;
    COLORREF        m_BkColor;

// Operations
public:

// Implementation
public:
    virtual ~CFolditView();
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnInitialUpdate();
    virtual void OnUpdate (CView *pSender, LPARAM lHint, CObject *pHint);
    BOOL PreCreateWindow(CREATESTRUCT& cs);
    int GetCurLine () const;
    int GetCurLength () const;
    void ReSizeView();
    BOOL IsHighlightedP () const;
    void Searchforinitial();
    BOOL IsKeyPadCharP(UINT nFlags) const;

    BOOL CanExtendRightP();
    void DoExtendRight();
    BOOL CanExtendUpDownP(int);
    void DoExtendUpDown(int);
    BOOL CanOpenRefP();
    void DoOpenRef();
    BOOL CanEditTaskP();
    void DoEditTask();
    BOOL CanEditCutP();
    BOOL CanEditCopyP();
    BOOL CanEditPasteP(BOOL Includeclipboard=FALSE);
    BOOL CanEditDeleteP();
    void DoEditCut();
    void DoEditCopy(BOOL Copytoclipboard=FALSE);
    void DoEditDelete();
    void DoEditPaste(BOOL Pastefromclipbopard=FALSE);
    BOOL CanEditCutlineP();
    BOOL CanEditCopylineP();
    BOOL CanEditPastelineP();
    BOOL CanEditDeletelineP();
    void DoEditCutline();
    void DoEditCopyline();
    void DoEditPasteline();
    void DoEditDeleteline();
    BOOL CanFoldCreateP();
    void DoFoldCreate();
    BOOL CanFoldRipP();
    void DoFoldRip();
    BOOL CanFoldUnfoldP();
    void DoFoldUnfold();
    void DoEditAppend();

    int GetCurIndent () const;
    BOOL GetSelInfo (int&, int&, int&, int&);

    BOOL CanFoldExitfoldP();
    void DoFoldExitfold();
    BOOL CanFoldEnterfoldP();
    void DoFoldEnterfold();
    BOOL CanDeleteCharP(int &CurLine, int &CurIndent);
    void DoDeleteChar();
    BOOL CanBackSpaceCharP();
    void DoBackSpaceChar();
    BOOL CanInsertCharP(BOOL &IsHL, POSITION &pos, int &CurLine, int &CurIndent, UINT c);
    void DoInsertChar(UINT c);
    BOOL CanSplitLineP();
    void DoSplitLine();
    BOOL CanCutCopyDelP (BOOL DoCut, BOOL DoCopy);
    void DoCutCopyDel (BOOL DoCut, BOOL DoCopy, BOOL CopyToClipboard=FALSE);
    void ClipCurLine();
    int CanClipCurLineP();
    int  InVirtualSpace();
    void DoInsertTab();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Printing support
protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

    // OLE Client support
    //virtual BOOL IsSelected(const CObject* pDocItem) const;

// Generated message map functions
protected:
    //{{AFX_MSG(CFolditView)
    afx_msg void OnOptionsFont();
    afx_msg void OnUpdateOptionsFont(CCmdUI* pCmdUI);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnFoldEnterfold();
    afx_msg void OnUpdateFoldEnterfold(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFoldExitfold(CCmdUI* pCmdUI);
    afx_msg void OnFoldExitfold();
    afx_msg void OnFoldEnterselectedfold();
    afx_msg void OnUpdateFoldEnterselectedfold(CCmdUI* pCmdUI);
    afx_msg void OnFoldCreate();
    afx_msg void OnUpdateFoldCreate(CCmdUI* pCmdUI);
    afx_msg void OnFoldRip();
    afx_msg void OnUpdateFoldRip(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFoldUnfold(CCmdUI* pCmdUI);
    afx_msg void OnFoldUnfold();
    afx_msg void OnEditDeleteline();
    afx_msg void OnUpdateEditDeleteline(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorLine(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorCol(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorRed(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorWho(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorLast(CCmdUI* pCmdUI);
    afx_msg void OnUpdateIndicatorDirty(CCmdUI* pCmdUI);
    afx_msg void OnOptionsEditing();
    afx_msg void OnUpdateOptionsEditing(CCmdUI* pCmdUI);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnEditCopy();
    afx_msg void OnEditCut();
    afx_msg void OnEditPaste();
    afx_msg void OnContextHelp();
    afx_msg void OnFileOpenreference();
    afx_msg void OnToolsedittask();
    afx_msg void OnUpdateFileOpenreference(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolsedittask(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg void OnDevtestShoeinternalpastebuffer();
    afx_msg void OnUpdateDevtestShoeinternalpastebuffer(CCmdUI* pCmdUI);
    afx_msg void OnEditDelete();
    afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
    afx_msg void OnDebugDisplayselectedtextparameters();
    afx_msg void OnUpdateDebugDisplayselectedtextparameters(CCmdUI* pCmdUI);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
    afx_msg void OnEditCutline();
    afx_msg void OnUpdateEditCutline(CCmdUI* pCmdUI);
    afx_msg void OnEditCopyline();
    afx_msg void OnUpdateEditCopyline(CCmdUI* pCmdUI);
    afx_msg void OnEditCopytoclipboard();
    afx_msg void OnUpdateEditCopytoclipboard(CCmdUI* pCmdUI);
    afx_msg void OnEditPastefromclipboard();
    afx_msg void OnUpdateEditPastefromclipboard(CCmdUI* pCmdUI);
    afx_msg void OnEditPasteline();
    afx_msg void OnUpdateEditPasteline(CCmdUI* pCmdUI);
    afx_msg void OnToolsTexttofold();
    afx_msg void OnUpdateToolsTexttofold(CCmdUI* pCmdUI);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnEditFindfirst();
    afx_msg void OnEditFindnext();
    afx_msg void OnUpdateEditFindfirst(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditFindnext(CCmdUI* pCmdUI);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnToolsStartkeyrecorder();
    afx_msg void OnUpdateToolsStartkeyrecorder(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolsStopkeyrecorder(CCmdUI* pCmdUI);
    afx_msg void OnToolsStopkeyrecorder();
    afx_msg void OnToolsPlaykeyrecording();
    afx_msg void OnUpdateToolsPlaykeyrecording(CCmdUI* pCmdUI);
    afx_msg void OnToolsInsertdate();
    afx_msg void OnToolsInserttitle();
    afx_msg void OnToolsInsertfindstring();
    afx_msg void OnUpdateToolsInsertdate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolsInserttitle(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolsInsertfindstring(CCmdUI* pCmdUI);
    afx_msg void OnToolsIndentalllines();
    afx_msg void OnUpdateToolsIndentalllines(CCmdUI* pCmdUI);
    afx_msg void OnToolsOutdentalllines();
    afx_msg void OnUpdateToolsOutdentalllines(CCmdUI* pCmdUI);
    afx_msg void OnPaint();
        afx_msg void OnToolsInsertnextserialnumber();
        afx_msg void OnUpdateToolsInsertnextserialnumber(CCmdUI* pCmdUI);
        //}}AFX_MSG
    DECLARE_MESSAGE_MAP()


};

#endif

