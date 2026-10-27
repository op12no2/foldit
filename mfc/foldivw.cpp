//{{{}}}

//{{{  history
//
// 3/4/97   CRJ Tweaked FindNext so that it detects a missing contexts which
//              was causing GPFs
// 3/4/97   CRJ Add insert title/find-string stuff
// 20/11/99 DCN Hack out line referencing un-initialised variables
//              Extend open ref to look in dir of last open too
// 1/12/99  DCN When create a file that doesn't exist do it in the same folder
//              as the parent document. (see OpenRef)
//              Use parent folder path not m_LastRed in OpenRef
// 1/2/100  DCN Adjust dates for Y2K (local time returns year# relative to 1900)
// 07/03/05 CRJ Add edit task stuff [7223].
// 31/05/07 DCN Use \ folder separator not \\ in OpenRef
//              Update task ref URL for make247.co.uk
//
//}}}

//{{{  includes

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>

#include "stdafx.h"
#include <limits.h>
#include <time.h>
#include "foldit.h"

#include "mainfrm.h"
#include "searchdl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}

//{{{  message map

IMPLEMENT_DYNCREATE(CFolditView, CEditView)

BEGIN_MESSAGE_MAP(CFolditView, CEditView)
    //{{AFX_MSG_MAP(CFolditView)
    ON_COMMAND(ID_OPTIONS_FONT, OnOptionsFont)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_FONT, OnUpdateOptionsFont)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDBLCLK()
    ON_COMMAND(ID_NAVIGATE_ENTERFOLD, OnFoldEnterfold)
    ON_UPDATE_COMMAND_UI(ID_NAVIGATE_ENTERFOLD, OnUpdateFoldEnterfold)
    ON_UPDATE_COMMAND_UI(ID_NAVIGATE_EXITFOLD, OnUpdateFoldExitfold)
    ON_COMMAND(ID_NAVIGATE_EXITFOLD, OnFoldExitfold)
    ON_COMMAND(ID_NAVIGATE_ENTERSELECTEDFOLD, OnFoldEnterselectedfold)
    ON_UPDATE_COMMAND_UI(ID_NAVIGATE_ENTERSELECTEDFOLD, OnUpdateFoldEnterselectedfold)
    ON_COMMAND(ID_FOLD_CREATE, OnFoldCreate)
    ON_UPDATE_COMMAND_UI(ID_FOLD_CREATE, OnUpdateFoldCreate)
    ON_COMMAND(ID_FOLD_RIP, OnFoldRip)
    ON_UPDATE_COMMAND_UI(ID_FOLD_RIP, OnUpdateFoldRip)
    ON_UPDATE_COMMAND_UI(ID_FOLD_UNFOLD, OnUpdateFoldUnfold)
    ON_COMMAND(ID_FOLD_UNFOLD, OnFoldUnfold)
    ON_COMMAND(ID_EDIT_DELETELINE, OnEditDeleteline)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETELINE, OnUpdateEditDeleteline)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINE, OnUpdateIndicatorLine)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_COL, OnUpdateIndicatorCol)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_RED, OnUpdateIndicatorRed)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_WHO, OnUpdateIndicatorWho)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_LAST, OnUpdateIndicatorLast)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_DIRTY, OnUpdateIndicatorDirty)
    ON_COMMAND(ID_OPTIONS_EDITING, OnOptionsEditing)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_EDITING, OnUpdateOptionsEditing)
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
    ON_COMMAND(ID_FILE_OPENREFERENCE, OnFileOpenreference)
    ON_UPDATE_COMMAND_UI(ID_FILE_OPENREFERENCE, OnUpdateFileOpenreference)
    ON_COMMAND(ID_TOOLS_EDITTASK, OnToolsedittask)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_EDITTASK, OnUpdateToolsedittask)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_COMMAND(ID_DEVTEST_SHOEINTERNALPASTEBUFFER, OnDevtestShoeinternalpastebuffer)
    ON_UPDATE_COMMAND_UI(ID_DEVTEST_SHOEINTERNALPASTEBUFFER, OnUpdateDevtestShoeinternalpastebuffer)
    ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
    ON_COMMAND(ID_DEBUG_DISPLAYSELECTEDTEXTPARAMETERS, OnDebugDisplayselectedtextparameters)
    ON_UPDATE_COMMAND_UI(ID_DEBUG_DISPLAYSELECTEDTEXTPARAMETERS, OnUpdateDebugDisplayselectedtextparameters)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_COMMAND(ID_EDIT_CUTLINE, OnEditCutline)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUTLINE, OnUpdateEditCutline)
    ON_COMMAND(ID_EDIT_COPYLINE, OnEditCopyline)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPYLINE, OnUpdateEditCopyline)
    ON_COMMAND(ID_EDIT_COPYTOCLIPBOARD, OnEditCopytoclipboard)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPYTOCLIPBOARD, OnUpdateEditCopytoclipboard)
    ON_COMMAND(ID_EDIT_PASTEFROMCLIPBOARD, OnEditPastefromclipboard)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTEFROMCLIPBOARD, OnUpdateEditPastefromclipboard)
    ON_COMMAND(ID_EDIT_PASTELINE, OnEditPasteline)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTELINE, OnUpdateEditPasteline)
    ON_COMMAND(ID_TOOLS_TEXTTOFOLD, OnToolsTexttofold)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_TEXTTOFOLD, OnUpdateToolsTexttofold)
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_EDIT_FINDFIRST, OnEditFindfirst)
    ON_COMMAND(ID_EDIT_FINDNEXT, OnEditFindnext)
    ON_UPDATE_COMMAND_UI(ID_EDIT_FINDFIRST, OnUpdateEditFindfirst)
    ON_UPDATE_COMMAND_UI(ID_EDIT_FINDNEXT, OnUpdateEditFindnext)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_RBUTTONDOWN()
    ON_WM_GETDLGCODE()
    ON_COMMAND(ID_TOOLS_STARTKEYRECORDER, OnToolsStartkeyrecorder)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_STARTKEYRECORDER, OnUpdateToolsStartkeyrecorder)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_STOPKEYRECORDER, OnUpdateToolsStopkeyrecorder)
    ON_COMMAND(ID_TOOLS_STOPKEYRECORDER, OnToolsStopkeyrecorder)
    ON_COMMAND(ID_TOOLS_PLAYKEYRECORDING, OnToolsPlaykeyrecording)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_PLAYKEYRECORDING, OnUpdateToolsPlaykeyrecording)
    ON_COMMAND(ID_TOOLS_INSERTDATE, OnToolsInsertdate)
    ON_COMMAND(ID_TOOLS_INSERTTITLE, OnToolsInserttitle)
    ON_COMMAND(ID_TOOLS_INSERTFINDSTRING, OnToolsInsertfindstring)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_INSERTDATE, OnUpdateToolsInsertdate)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_INSERTTITLE, OnUpdateToolsInserttitle)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_INSERTFINDSTRING, OnUpdateToolsInsertfindstring)
    ON_COMMAND(ID_TOOLS_INDENTALLLINES, OnToolsIndentalllines)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_INDENTALLLINES, OnUpdateToolsIndentalllines)
    ON_COMMAND(ID_TOOLS_OUTDENTALLLINES, OnToolsOutdentalllines)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_OUTDENTALLLINES, OnUpdateToolsOutdentalllines)
    ON_WM_PAINT()
    ON_COMMAND(ID_TOOLS_INSERTNEXTSERIALNUMBER, OnToolsInsertnextserialnumber)
    ON_UPDATE_COMMAND_UI(ID_TOOLS_INSERTNEXTSERIALNUMBER, OnUpdateToolsInsertnextserialnumber)
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()
//}}}

//{{{  constructor/destructor

CFolditView::CFolditView()
{
    m_pDoc = NULL;
    m_pFont                 = NULL;
    m_pCurrent              = NULL;
    m_DefaultHint.m_Sel     = (DWORD)0;
    m_DefaultHint.m_TopLine = 0;
    m_BkColor = App()->m_DefBkGnd;
    m_BkBrush.CreateSolidBrush(m_BkColor);
    m_cf.rgbColors = App()->m_DefFrGnd;
}

CFolditView::~CFolditView()
{
    ((CMainFrame *)(App()->m_pMainWnd))->m_pFocus = NULL;

    if (m_pCurrent)
      m_pCurrent->Release();

    if (m_pFont)
        delete m_pFont;

    POSITION pos = m_OnExitFifo.GetHeadPosition();

    while (pos)
    {
        delete m_OnExitFifo.GetNext(pos);
    }

    m_OnExitFifo.RemoveAll();
}

//}}}

//{{{  helpers

//{{{  ReSizeView

void CFolditView::ReSizeView()
{
    //RECT r;
    //GetParent()->GetWindowRect(&r);
    //GetParent()->GetParent()->ScreenToClient (&r);

    //TEXTMETRIC tm;
    //CClientDC dc(GetParent());
    //dc.SelectObject (m_pFont);
    //dc.GetTextMetrics(&tm);
    //int CharHeight   = tm.tmHeight + tm.tmExternalLeading;
    //int CharWidth    = tm.tmAveCharWidth;
    //int CaptionWidth = 15;
    //int ExtraLines   = 1;

    //r.bottom = r.top + CaptionWidth + (GetEditCtrl().GetLineCount()+ExtraLines) * CharHeight;

    //GetParent()->MoveWindow (&r);
}

//}}}
//{{{  GetSelInfo
BOOL CFolditView::GetSelInfo (int &StartLine, int &StartChar,
                  int &EndLine,   int &EndChar)
{
    int StartSel, EndSel;

    GetEditCtrl().GetSel (StartSel, EndSel);

    StartLine = GetEditCtrl().LineFromChar(StartSel);
    StartChar = StartSel - GetEditCtrl().LineIndex(StartLine);
    EndLine = U_ERR;
    EndChar = U_ERR;

    if (StartSel == EndSel)
    return FALSE;

    EndLine = GetEditCtrl().LineFromChar(EndSel);
    EndChar = EndSel - GetEditCtrl().LineIndex(EndLine);

    if ((StartChar == GetEditCtrl().LineLength(StartSel)) && (StartChar > 0))
    {
    //must be at least 2 lines selected
    StartChar = 0;
    StartLine++;

    if ((StartLine == EndLine) && (StartChar == EndChar))
        return FALSE;
    }

    if ((EndChar == 0) && (GetEditCtrl().LineLength(EndSel) > 0))
    {
    //must be at least 2 lines selected
    EndLine--;
    EndChar = GetEditCtrl().LineLength(EndSel-1);
    }

    if (EndChar > 0)
    EndChar--;

    ASSERT (EndLine >= 0);
    ASSERT (StartLine >= 0);
    ASSERT (EndChar >= 0);

    return TRUE;
}
//}}}
//{{{  GetCurLine

int CFolditView::GetCurLine() const
{
    return GetEditCtrl().LineFromChar(-1);
}
//}}}
//{{{  GetCurLength

int CFolditView::GetCurLength() const
{
    return GetEditCtrl().LineLength(-1);
}
//}}}
//{{{  GetCurIndent

int CFolditView::GetCurIndent() const
{
    CEdit *pEdit = &GetEditCtrl();

    int StartSel, EndSel;
    pEdit->GetSel (StartSel, EndSel);

    int TopSelLine = pEdit->LineFromChar(StartSel);

    return (StartSel - pEdit->LineIndex(TopSelLine));
}
//}}}
//{{{  IsHighlightedP

BOOL CFolditView::IsHighlightedP () const
{
    int StartSel, EndSel;

    GetEditCtrl().GetSel (StartSel, EndSel);

    return (StartSel != EndSel);
}
//}}}
//{{{  IsKeyPadCharP

BOOL CFolditView::IsKeyPadCharP (UINT nFlags) const
{
    return (nFlags & U_NUMERIC_PAD_MASK) == U_NUMERIC_PAD_MASK;
}
//}}}

//}}}
//{{{  framework overrides

//{{{  OnPaint
void CFolditView::OnPaint()
{
    CEditView::OnPaint();
    OnDraw(NULL);
}
//}}}
//{{{  OnDraw
void CFolditView::OnDraw(CDC* pDC)
{
  //int x=50;
  //char y[128];
  //if (pDC->GetTextFace(x,y))
  //  AfxMessageBox("y");
  //else
  //  AfxMessageBox("failed");
  //pDC->TextOut(0,0,"ABCDEFGHIJKLM");
  //pDC->LineTo(50,50);
  //pDC->MoveTo(0,50);
}

//}}}
//{{{  OnXPrinting
BOOL CFolditView::OnPreparePrinting(CPrintInfo* pInfo)
{
    //pInfo->SetMaxPage(1);
    return DoPreparePrinting(pInfo);
}

void CFolditView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  CEditView::OnBeginPrinting(pDC,pInfo);
}

void CFolditView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
  CEditView::OnEndPrinting(pDC,pInfo);
}

void CFolditView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
  CString head;
  char *p = head.GetBuffer(100);
  sprintf(p,"Page %d of ",pInfo->m_nCurPage);
  head.ReleaseBuffer();
  head = head + m_pDoc->GetTitle()+" :: "+m_pCurrent->m_Body;
  pDC->TextOut(0,0,head);
  pDC->MoveTo(0,50);
  pDC->LineTo(pInfo->m_rectDraw.right,50);
  pInfo->m_rectDraw.top += 70;
  //pDC->SetWindowOrg(pInfo->m_rectDraw.left,pInfo->m_rectDraw.top);
  CEditView::OnPrint(pDC,pInfo);
}
//}}}
//{{{  OnInitialUpdate
void CFolditView::OnInitialUpdate()
{
    //get pointer to attached document
    m_pDoc = (CFolditDoc *)GetDocument();

    if (m_pCurrent == NULL)
        //get pointer to top level fold in doc
        m_pCurrent = m_pDoc->GetTop();
    else
        //pointer already setup by another view
        ;

    m_pCurrent->Acquire();

    //set default font
    memset (&m_lf, 0, sizeof(LOGFONT));

    //get name from .ini file (now in app)
    lstrcpy (m_lf.lfFaceName, App()->m_DefaultFontName);

    //get height from .ini file (now in app)
    m_lf.lfHeight = App()->m_DefaultFontHeight;

    if (m_pFont)
        delete m_pFont;

    m_pFont = new CFont;
    MEM(m_pFont);

    if (m_pFont)
    {
      if (!m_pFont->CreateFontIndirect(&m_lf))
      {
        //failed to create new font so leave as windows default
        ::GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &m_lf);
        VERIFY(m_pFont->CreateFontIndirect(&m_lf));
      }
      GetEditCtrl().SetFont(m_pFont);
    }
    //do first view rendition
    CView::OnInitialUpdate();
}
//}}}
//{{{  OnUpdate
void CFolditView::OnUpdate (CView *pSender, LPARAM lHint, CObject *pHint)
{
    //RECT rect;
    //GetEditCtrl().GetRect(&rect);
    //rect.left += 50;
    //rect.top += 50;
    //GetEditCtrl().SetRectNP(&rect);

    CHintBlind *pHintInfo = (CHintBlind *)pHint;

    //only update if current fold is affected
    //however, always update if pHint is NULL (MFC default)
    if (pHintInfo != NULL)
    {
        if (pHintInfo->m_pCurrent != m_pCurrent)
            //not me - using a different fold
            return;

        else
            ;
    }
    else
    {
        pHintInfo = &m_DefaultHint;
        lHint = HINT_BLIND;
    }

    //now always a valid hint pointer in pHintInfo
    switch (lHint)
    {
        case HINT_BLIND:
        {
            //decremental buffer contents indicator;
            unsigned int BufSize   = U_UPDATEBUFFER;
            unsigned int TotalSize = 0;

            //m_pDoc->BeginWaitCursor();

            //get pointer to edit control
            CEdit *pEdit = &GetEditCtrl();

            //the new edit text which will replace the existing text
            CString FoldText;

            //Buffer for each line during iteration through fold
            CString FoldLineText;


            FoldText.GetBuffer (BufSize);

            for (POSITION pos = m_pCurrent->m_LineList.GetHeadPosition(); pos != NULL;)
            {
                FoldLineText.Empty();

                //treat each line as an abstract 'line' and manipulate with virtual functions
                CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetNext(pos);
                ASSERT (pLine);

                //get the line to render itself into a string (including fold markers etc)
                //assume that the length of the line is less than the
                //capabilities of a CString at this stage
                pLine->Render(FoldLineText);

                TotalSize += FoldLineText.GetLength();

                if (TotalSize >= BufSize)
                {
                    //compute new buffer size
                    BufSize = TotalSize + U_UPDATEBUFFER;

                    if (BufSize >= INT_MAX)
                      BufSize = INT_MAX - 1;

                    TRACE1 ("New Update Buffer Size = %ld\n", (int)BufSize);

                    if (TotalSize >= BufSize)
                    {
                        ImplementationRestriction("Cannot fit whole fold in window.\n"
                                      "Folding some of the displayed text will help.");
                        break;
                    }

                    //extend buffer space
                    FoldText.ReleaseBuffer();

                    TRY
                    {
                        FoldText.GetBuffer (BufSize);
                    }
                    CATCH_ALL(e)
                    {
                        EnvironmentRestriction ("Not enough memory available to display whole fold.");
                        break;
                    }
                    END_CATCH_ALL
                }

                //add rendered line to fold string
                TRY
                {
                    FoldText += FoldLineText;
                }
                CATCH_ALL(e)
                {
                    EnvironmentRestriction ("Not enough memory available to display whole fold.");
                    break;
                }
                END_CATCH_ALL
            }

            FoldText.ReleaseBuffer();

            //remove last CRLF
            TRY
            {
                //FoldText.GetBufferSetLength(FoldText.GetLength()-2);
                //FoldText.ReleaseBuffer();
                //ASSERT (FoldText.GetLength() >= 0);
            }
            CATCH_ALL(e)
            {
                EnvironmentRestriction ("Not enough memory available to display fold.");
            }
            END_CATCH_ALL

            FoldText = FoldText + App()->m_EndFoldCue;
            //m_pDoc->EndWaitCursor();

            //we now have the text for the edit box, the top line to
            //scroll to and the selection to use after insertion in the box
            //will have been supplied by the hint object, or if NULL use defaults

            //disable Windows from doing incremental updates to the edit box
            pEdit->SetRedraw(FALSE);

            //set the text to that just constructed
            pEdit->SetWindowText(FoldText);

            //set the selection sepcified in the hint (inhibit scroll)
            if (pHintInfo->m_Sel != ((DWORD)0))
                pEdit->SetSel(pHintInfo->m_Sel, TRUE);
            else
            {
                int CharIndex = pEdit->LineIndex (pHintInfo->m_TopLine);
                pEdit->SetSel(CharIndex, CharIndex, TRUE);

                //adjust the top line to give some context
                pHintInfo->m_TopLine -= 7;
                if (pHintInfo->m_TopLine < 0)
                  pHintInfo->m_TopLine = 0;
            }

            //scroll to the specified line (will become top line in view)
            pEdit->LineScroll(pHintInfo->m_TopLine);

            //display the updated text, caret and top line
            pEdit->SetRedraw(TRUE);

            //Set title to fold title
            GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + m_pCurrent->m_Body);

            break;
        }

        case HINT_REPLACE:
        {
            CEdit *pEdit = &GetEditCtrl();

            ASSERT (pHintInfo);

            //coerce generic blind hint into 'replace' hint
            CHintReplace *pReplace = (CHintReplace *)pHintInfo;

            //do not do a redraw inhibit as this causes flashing when re-enabled
            //bacause Windows does an invalidate on the whole edit window
            //HACK could enhance this
            //pEdit->SetRedraw(FALSE);

            //hide the caret to help stop flickering temp highlight
            //HACK does not seem to work, ie does not hide the sel below
            //pEdit->HideCaret();

            //select the text to replace (do not scroll into view yet - TRUE param)
            pEdit->SetSel (pReplace->m_StartSel, pReplace->m_EndSel, TRUE);

            //replace the current selection
            pEdit->ReplaceSel(pReplace->m_Chars);

            //scroll to post edit specified line so that all windows match
            //This is the part of the update routine that slugs it (x2) - avoid
            //if possible - caller puts < 0 in m_TopLine to do this.
            if (pReplace->m_TopLine >= 0)
            {
                int nLine = pEdit->GetFirstVisibleLine();
                pEdit->LineScroll (pReplace->m_TopLine - nLine);
            }

            //set specified post edit sel (no scroll)
            pEdit->SetSel (pReplace->m_Sel, TRUE);
            //and restore caret
            //pEdit->ShowCaret();

            //pEdit->SetRedraw(TRUE);

            //Set title to fold title
            GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + m_pCurrent->m_Body);

            break;
        }
    }
    Searchforinitial();
}

//}}}
//{{{  OnCtlColor
HBRUSH CFolditView::OnCtlColor (CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    TRACE0 ("CFoldItView::OnCtlColor\n");

    return CEditView::OnCtlColor(pDC, pWnd, nCtlColor);
}
//}}}
//{{{  PreCreateWindow
BOOL CFolditView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CEditView::PreCreateWindow(cs);
}
//}}}
//{{{  OnEraseBkgrnd
BOOL CFolditView::OnEraseBkgnd(CDC* pDC)
{
    return CEditView::OnEraseBkgnd(pDC);
}

//}}}
//{{{  OnSetFocus

void CFolditView::OnSetFocus(CWnd* pOldWnd)
{
    CEditView::OnSetFocus(pOldWnd);

    //if (m_pDoc)
    //    GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + m_pCurrent->m_Body);
    // TODO: Add your message handler code here

}

//}}}
//{{{  OnKillFocus

void CFolditView::OnKillFocus(CWnd* pNewWnd)
{
    CEditView::OnKillFocus(pNewWnd);

    //if (m_pDoc)
    //    GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + m_pCurrent->m_Body);
    // TODO: Add your message handler code here

}

//}}}
//{{{  OnGetDlgCode

UINT CFolditView::OnGetDlgCode()
{
    // TODO: Add your message handler code here and/or call default

    return CEditView::OnGetDlgCode();
}
//}}}

//{{{  Diagnostics

/////////////////////////////////////////////////////////////////////////////
// CFolditView diagnostics

#ifdef _DEBUG
void CFolditView::AssertValid() const
{
    CView::AssertValid();
}

void CFolditView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

#endif //_DEBUG
//}}}

//}}}
//{{{  commands

//{{{  EditTask

//{{{  CanP

BOOL CFolditView::CanEditTaskP()
{
    if (IsHighlightedP())
    return FALSE;

    //look for a task ref
    CLine *pLine = m_pCurrent->GetLineAt(GetCurLine());
    if (!pLine) return FALSE;

    CString rfile;
    if (pLine->m_Body.ExtractBetween('[', ']', rfile))
      if (atoi(rfile))
        return TRUE;

    return FALSE;
}

//}}}
//{{{  Do

//try to edit a task

void CFolditView::DoEditTask()
{
    //{{{  check its a task ref first
    
    if (!CanEditTaskP())
      return;
    
    //}}}
    //{{{  get task and early returns
    
    CLine *pLine = m_pCurrent->GetLineAt(GetCurLine());
    if (!pLine) return;
    
    CString rfile;
    
    if (!pLine->m_Body.ExtractBetween('[', ']', rfile))
      return;
    
    if (!atoi(rfile))
      return;
    
    //}}}
    //{{{  edit it
    
    //AfxMessageBox(rfile);
    
    HWND    hwnd;
    CString url;
    
    hwnd = App()->m_pMainWnd->GetSafeHwnd();
    url  = "http://admin.make247.co.uk/tasks.php?act=updateform&handle=" + rfile;
    
    ShellExecute(hwnd,NULL,url,NULL,NULL,1);
    
    //}}}
    return;
}

//}}}

//}}}
//{{{  OpenRef

//{{{  CanP

BOOL CFolditView::CanOpenRefP()
{
    if (IsHighlightedP())
    return FALSE;

    if (m_pCurrent->IsEnterableAtP (GetCurLine()))
    return FALSE;

    if (App()->m_NavEnterRefP)
    {
        //look for file name
        CLine *pLine = m_pCurrent->GetLineAt(GetCurLine());
        if (!pLine) return FALSE;

        CString rfile;
        if (pLine->m_Body.ExtractBetween('"', rfile))
            return TRUE;
        if (pLine->m_Body.ExtractBetween('\'', rfile))
            return TRUE;
    }
    return FALSE;
}

//}}}
//{{{  Do

//try to open a filed fold

void CFolditView::DoOpenRef()
{

    //{{{  check its a file ref first
    
    if (!CanOpenRefP())
        return;
    
    //}}}
    //{{{  look for file name
    
    CString search;
    
    //look for file name
    
    CLine *pLine = m_pCurrent->GetLineAt(GetCurLine());
    if (!pLine) return;
    
    if (!pLine->m_Body.ExtractBetween('@', search))
      search = "";
    App()->m_InitFind = search;
    
    CString rfile;
    FILE *f;
    
    if (!pLine->m_Body.ExtractBetween('\'', rfile))
      pLine->m_Body.ExtractBetween('"', rfile);
    
    //}}}

    //{{{  look in parent folder first
    if ((f = fopen(m_pDoc->m_Folder + rfile,"r")) != NULL)
    {
        //exists in parent folder
        fclose(f);
        if (App()->Exec(m_pDoc->m_Folder + rfile)) return;
        App()->OpenDocumentFile (m_pDoc->m_Folder + rfile);
        return;
    }
    //}}}
    //{{{  look in logged dir next
    if ((f = fopen(rfile,"r")) != NULL)
    {
        //exists in logged dir or is abs
        fclose(f);
        if (App()->Exec(rfile)) return;
        App()->OpenDocumentFile(rfile);
        return;
    }
    //}}}
    //{{{  look in the reds next
    for (int i=0; i<App()->m_Red.GetSize(); i++)
    {
       if ((f = fopen(App()->m_Red.GetAt(i) + '\\' + rfile,"r")) != NULL)
       {
         //found it
         fclose(f);
         if (App()->Exec(App()->m_Red.GetAt(i) + '\\' + rfile)) return;
         App()->OpenDocumentFile(App()->m_Red.GetAt(i) + '\\' + rfile);
         return;
       }
    }
    //}}}

    //{{{  doesn't exist, make path absolute
    
    //try to create the file in the same folder as our parent file
    
    if(rfile[0] == '\\' || rfile[1] == ':')
      ; //its an absolute path, leave as is
    else
    {
      //its a relative path, prepend the parent folder
      rfile = m_pDoc->m_Folder + rfile;
    }
    
    //}}}

    //{{{  try for an association last
    
    //if there's an association, its that apps problem to create the file
    //not ours.
    
    if (App()->Exec(rfile)) return;
    
    //}}}

    //{{{  doesn't exist, set to create it
    
    App()->OpenDocumentFile(rfile);
    
    //}}}

    return;

}

//}}}

//}}}
//{{{  FoldEnterfold

//{{{  CanP

BOOL CFolditView::CanFoldEnterfoldP()
{
    //if (IsHighlightedP())
    //    return FALSE;

    if (m_pCurrent->IsEnterableAtP (GetCurLine()))
        return TRUE;

    return FALSE;
}

//}}}
//{{{  Do

void CFolditView::DoFoldEnterfold()
{
    if (!CanFoldEnterfoldP())
         return;

    if (App()->m_AlwaysRip)
    {
        DoFoldRip();
        return;
    }

    CFoldLine *pTmp = m_pCurrent;

    //push the current caret position and scroll position onto the exit stack
    //as a hint which is used when the fold is returned to

    CHintBlind *pHint = new CHintBlind;
    MEM(pHint);

    //release the present fold
    m_pCurrent->Release();

    pHint->m_pCurrent       = m_pCurrent;
    pHint->m_Sel            = GetEditCtrl().GetSel();
    pHint->m_TopLine        = GetEditCtrl().GetFirstVisibleLine();

    //replace current with child
    m_pCurrent = (CFoldLine *)m_pCurrent->GetLineAt (GetCurLine());
    if (!m_pCurrent)
    {
       m_pCurrent = pTmp;
       m_pCurrent->Acquire();
       delete pHint;
       return;
    }

    m_OnExitFifo.AddTail (pHint);
    //mark this fold as being used
    m_pCurrent->Acquire();

    //Check that it has at least one line in it - backwards compat with
    //older versions
    if (m_pCurrent->m_LineList.IsEmpty())
        m_pCurrent->m_LineList.AddTail (new CTextLine);

    //(use the default blind hint)
    OnUpdate(NULL, HINT_BLIND, NULL);
    //delete pHint;
    ReSizeView();
}

//}}}

//}}}
//{{{  FoldExitfold

//{{{  CanP

BOOL CFolditView::CanFoldExitfoldP()
{
    if (m_pCurrent->IsExitableP())
        return TRUE;

    //must be at top level
    if (App()->m_NavAutoCloseP )
        return TRUE;

    return FALSE;
}

//}}}
//{{{  Do

void CFolditView::DoFoldExitfold()
{
    if (!CanFoldExitfoldP())
        return;

    if (App()->m_DelFoldTails)
      m_pCurrent->RemoveTrailing();

    if (!(m_pCurrent->IsExitableP()))
    {
        if (m_pCurrent->m_UsageCount > 1)
        {
          //ditch this view
          ((CFolditMDIChildWnd *)GetParent())->MDIDestroy();
          return;
        }
        else
        //at top level with auto close set
        //App()->m_pMainWnd->PostMessage (ID_FILE_CLOSE);
        if (!m_pDoc->m_Sticky)
            m_pDoc->OnFileClose();
        //GetParent()->PostMessage(ID_FILE_CLOSE);
        return;
    }

    ASSERT (m_pCurrent->m_pParent);

    if (m_OnExitFifo.IsEmpty())
    {
        //reached root of ripped fold (destructor will release fold)
        if (m_pCurrent->m_pParent->IsActiveP())
        {
            //commit hari-kari
            ((CFolditMDIChildWnd *)GetParent())->MDIDestroy();
            return;
        }

        //HACK experiment - if no parent then go 'up' the fold structure
        //useful after a find

        int ChildLine = 0;
        POSITION pos = m_pCurrent->m_pParent->m_LineList.GetHeadPosition();
        while (1)
        {
          if (m_pCurrent->m_pParent->m_LineList.GetNext(pos) == m_pCurrent)
            break;
          ChildLine++;
        }

        //release the fold from this view (may still be in use by other stuff)
        m_pCurrent->Release();

        //Not at root yet but no fifo
        m_pCurrent = m_pCurrent->m_pParent;
        m_pCurrent->Acquire();

        CHintBlind *pHint = new CHintBlind;
        MEM(pHint);

        pHint->m_pCurrent       = m_pCurrent;
        pHint->m_Sel            = (DWORD)0;
        pHint->m_TopLine        = ChildLine;

        OnUpdate(NULL, HINT_BLIND, pHint);

        delete pHint;
    }

    else
    {
        //release the fold from this view (may still be in use by other stuff)
        m_pCurrent->Release();

        //Not at root yet or not ripped fold
        m_pCurrent = m_pCurrent->m_pParent;
        m_pCurrent->Acquire();

        //redraw JUST this view with the saved sel/topline
        CHintBlind *pHint = (CHintBlind *)m_OnExitFifo.RemoveTail();

        OnUpdate(NULL, HINT_BLIND, pHint);

        delete pHint;
    }
}

//}}}

//}}}
//{{{  FoldCreate

//{{{  CanP

BOOL CFolditView::CanFoldCreateP()
{
    int StartLine, EndLine, StartChar, EndChar;

    BOOL IsHighlighted = GetSelInfo (StartLine, StartChar, EndLine, EndChar);

    if (!IsHighlighted)
        return TRUE;
    else
        return m_pCurrent->CanCreateChildFoldAtP(StartLine, EndLine - StartLine + 1, StartChar);
}

//}}}
//{{{  Do
void CFolditView::DoFoldCreate()
{
    if (!CanFoldCreateP())
        return;

    CEdit *pEdit = &GetEditCtrl();

    int StartLine, EndLine, StartChar, EndChar;

    BOOL IsHighlighted = GetSelInfo (StartLine, StartChar, EndLine, EndChar);

    if (!IsHighlighted)
    {
        m_pCurrent->CreateChildFoldAt (StartLine, 0, StartChar);
    }

    else
    {
        //some text is selected
        //compute number of lines to fold up
        int nLines = EndLine - StartLine + 1;

        //and fold
        m_pCurrent->CreateChildFoldAt (StartLine, nLines, StartChar);
    }

    //mark the doc as dirty
    m_pDoc->SetModifiedFlag();

    //fold is now created but not displayed;
    //set up a blind hint so that the new selecetion will be the default title of the new fold

    //StartLine will contain the new fold index; the start of the line can be found easily
    int NewCaretPos = pEdit->LineIndex(StartLine);

    //get a pointer to the new fold
    CFoldLine *pNew = (CFoldLine *)m_pCurrent->GetLineAt (StartLine);
    if (!pNew) return;

    //add the size of its indent to the caret position
    NewCaretPos += pNew->m_Indent;

    //add the length of the prefix to the caret position
    NewCaretPos += pNew->GetPrefix().GetLength() + 2;

    CHintBlind *pHint = new CHintBlind;
    MEM(pHint);

    pHint->m_pCurrent = m_pCurrent;
    pHint->m_TopLine  = pEdit->GetFirstVisibleLine();
    pHint->m_Sel      = MAKELONG (NewCaretPos, NewCaretPos + pNew->m_Body.GetLength());

    m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);

    delete pHint;
}
//}}}

//}}}
//{{{  FoldRip

//{{{  CanP

BOOL CFolditView::CanFoldRipP()
{
    if (IsHighlightedP())
    return FALSE;

    if (m_pCurrent->IsEnterableAtP (GetCurLine()))
    return TRUE;

    return FALSE;
}

//}}}
//{{{  Do

void CFolditView::DoFoldRip()
{
    if (!CanFoldRipP())
        return;

    CFolditMDIChildWnd* pActiveChild = (CFolditMDIChildWnd *)GetParent();
    ASSERT (pActiveChild);

    CDocument *pDocument = pActiveChild->GetActiveDocument();
    ASSERT (pDocument);

    CDocTemplate* pTemplate = pDocument->GetDocTemplate();
    ASSERT_VALID(pTemplate);

    CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
    ASSERT (pFrame);
    MEM (pFrame);
    CFolditView *pView;

    if (pFrame->GetActiveView() == NULL)
    {
        CWnd* pWnd = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST);

        if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
        {
            //set the new view as active
            pFrame->SetActiveView((CView*)pWnd);

            pView = (CFolditView *)pWnd;
            pView->m_pCurrent = (CFoldLine *)m_pCurrent->GetLineAt(GetCurLine());
            if (!pView) return;

                 //Check that it has at least one line in it - backwards compat with
                     //older versions
            if (pView->m_pCurrent->m_LineList.IsEmpty())
                pView->m_pCurrent->m_LineList.AddTail (new CTextLine);
        }
    }

    pTemplate->InitialUpdateFrame(pFrame, pDocument);
    pView->GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + pView->m_pCurrent->m_Body);
}

//}}}

//}}}
//{{{  FoldUnfold

//{{{  CanP

BOOL CFolditView::CanFoldUnfoldP()
{
    if (IsHighlightedP())
    return FALSE;

    return m_pCurrent->CanUnFoldAtP (GetCurLine());
}

//}}}
//{{{  Do

void CFolditView::DoFoldUnfold()
{
    if (!CanFoldUnfoldP())
    {
        return;
    }

    m_pCurrent->UnFoldAt (GetCurLine());

        //view issue - dont allow an empty fold
        if (m_pCurrent->m_LineList.IsEmpty())
          m_pCurrent->m_LineList.AddTail (new CTextLine);

        //unfolded OK
        m_pDoc->SetModifiedFlag();

        CHintBlind *pHint = new CHintBlind;
        MEM(pHint);

        pHint->m_pCurrent = m_pCurrent;
        pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

        //force selection to first char on line (HACK, do more elegantly)
        pHint->m_Sel = GetEditCtrl().LineIndex();
        pHint->m_Sel = MAKELONG (pHint->m_Sel, pHint->m_Sel);

        m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);

        delete pHint;
}

//}}}

//}}}
//{{{  DeleteChar

//{{{  CanP

inline BOOL CFolditView::CanDeleteCharP (int &CurLine, int &CurIndent)
{
    CurLine   = GetCurLine();
    CurIndent = GetCurIndent();

    return m_pCurrent->CanDeleteCharAtP (CurLine, CurIndent);
}

//}}}
//{{{  Do

void CFolditView::DoDeleteChar ()
{
    int IsHL = IsHighlightedP();
    int CurLine, CurIndent;

    if (IsHL)
    {
         DoEditDelete();
         return;
    }

    //if (InVirtualSpace())  //HACK
    //  ClipCurLine();

    if (!CanDeleteCharP(CurLine, CurIndent))
        return;

    CEdit *pEdit = &GetEditCtrl();

    m_pCurrent->DeleteCharAt (CurLine, CurIndent);

    //view issue - dont allow an empty fold
    if (m_pCurrent->m_LineList.IsEmpty())
        m_pCurrent->m_LineList.AddTail (new CTextLine);

    //doc has changed
    m_pDoc->SetModifiedFlag();

    //create an appropriate hint for the update
    CHintReplace *pDelReplace = new CHintReplace;
    MEM(pDelReplace);

    pDelReplace->m_pCurrent = m_pCurrent;
    pDelReplace->m_Chars.Empty();

    //get selection and extend to cover char to be deleted
    pEdit->GetSel (pDelReplace->m_StartSel, pDelReplace->m_EndSel);

    pDelReplace->m_Sel     = MAKELONG (pDelReplace->m_StartSel, pDelReplace->m_EndSel);

    //only set top line if > 1 view - performance optimisation
    if (m_pDoc->GetViewCount() == 1)
        pDelReplace->m_TopLine = U_ERR;
    else
        pDelReplace->m_TopLine = pEdit->GetFirstVisibleLine();

    //bump the end sel past the deleted character
    pDelReplace->m_EndSel++;

    //bump pass LF if at end of line
    if (GetCurLength() <= CurIndent)
    {
        pDelReplace->m_EndSel++;
    }

    //redraw all other views
    m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pDelReplace);

    delete pDelReplace;

    return;
}

//}}}

//}}}
//{{{  BackSpaceChar

//{{{  CanP

BOOL CFolditView::CanBackSpaceCharP ()
{
    if (IsHighlightedP())
        return FALSE;

    CEdit *pEdit = &GetEditCtrl();

    int StartSel, EndSel;

    int CurLine   = GetCurLine();
    int CurIndent = GetCurIndent();

    //get current location of caret
    pEdit->GetSel (StartSel, EndSel);

    //special case at start of text
    if (StartSel == 0)
        return FALSE;

    //adjust caret for BS key
    if (CurIndent > 0)
    {
        //Not at SOL
        CurIndent--;
    }
    else
    {
        //AT SOL
        CurLine--;
        CurIndent = pEdit->LineLength(CurLine);
    }

    return m_pCurrent->CanDeleteCharAtP (CurLine, CurIndent);
}

//}}}
//{{{  Do

void CFolditView::DoBackSpaceChar ()
{
    if (!CanBackSpaceCharP())
        return;

    CEdit *pEdit = &GetEditCtrl();

    int StartSel, EndSel;
    pEdit->GetSel (StartSel, EndSel);

    int CurIndent = GetCurIndent();

    //adjust caret for BS key
    if (CurIndent > 0)
        pEdit->SetSel (StartSel-1, EndSel-1);
    else
        pEdit->SetSel (StartSel-2, EndSel-2);

    DoDeleteChar();
}

//}}}

//}}}
//{{{  DoInsertTab
void CFolditView::DoInsertTab ()
{
    int i;
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;

    if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, ' '))
        return;

    if (IsHL)
    {
        DoEditDelete();
        if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, ' '))
            return;
    }

    ASSERT (!IsHL);

    int CurLength   = GetCurLength();
    int LineOffset  = GetEditCtrl().LineIndex (CurLine);

    pos = m_pCurrent->m_LineList.FindIndex(CurLine);
    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

    int Extra = m_pDoc->m_pLanguage->m_Tab - (CurIndent % m_pDoc->m_pLanguage->m_Tab);

    for (i=0; i<Extra; i++)
      pLine->InsertChar(CurIndent+i,' ');

    //create an appropriate hint for the update
    CHintReplace *pReplace = new CHintReplace;
    MEM(pReplace);

    pReplace->m_pCurrent = m_pCurrent;
    pReplace->m_Chars    = CString(' ',Extra);
    pReplace->m_StartSel = LineOffset+CurIndent;
    pReplace->m_EndSel   = LineOffset+CurIndent;
    pReplace->m_Sel      = MAKELONG (LineOffset+CurIndent+Extra, LineOffset+CurIndent+Extra);
    pReplace->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

    m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);

    delete pReplace;

    m_pDoc->SetModifiedFlag();
}

//}}}
//{{{  InsertChar

//{{{  CanP

BOOL CFolditView::CanInsertCharP (BOOL &IsHL, POSITION &pos, int &CurLine, int &CurIndent, UINT nChar)
{
    IsHL = IsHighlightedP();

    if (IsHL)
        return CanEditDeleteP();

    CurLine   = GetCurLine();
    CurIndent = GetCurIndent();

    pos = m_pCurrent->CanInsertCharAtP (CurLine, CurIndent, nChar);

    if (pos)
        return TRUE;
    else
        return FALSE;
}

//}}}
//{{{  Do

void CFolditView::DoInsertChar (UINT nChar)
{
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;

    if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, nChar))
        return;

    if (IsHL)
    {
        DoEditDelete();
        if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, nChar))
            return;
    }

    ASSERT (!IsHL);
    m_pCurrent->InsertCharAt (pos, CurIndent, nChar);

    m_pDoc->SetModifiedFlag();

    //update local edit box with char
    if (!App()->m_KeyMacroPlayBack)
        CEditView::OnChar (nChar, 1, 0);   //HACK flags??

    if ((m_pDoc->GetViewCount() == 1) && (!App()->m_KeyMacroPlayBack))
    {
        return;
    }

    //create an appropriate hint for the update
    CHintReplace *pReplace = new CHintReplace;
    MEM(pReplace);

    pReplace->m_pCurrent    = m_pCurrent;
    pReplace->m_Chars       = CString((char)nChar);

    GetEditCtrl().GetSel (pReplace->m_StartSel, pReplace->m_EndSel);

    //since we've already put the char in the local view - we need
    //to adjust the current char position for the other views
    if (!App()->m_KeyMacroPlayBack)
    {
        pReplace->m_StartSel--;
        pReplace->m_EndSel--;
    }
    pReplace->m_Sel = MAKELONG (pReplace->m_StartSel+1, pReplace->m_EndSel+1);
    pReplace->m_TopLine = GetEditCtrl().GetFirstVisibleLine();

    //redraw non-local views
    if (App()->m_KeyMacroPlayBack)
        m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);
    else
        m_pDoc->UpdateAllViews (this, HINT_REPLACE, pReplace);

    delete pReplace;
}

//}}}

//}}}
//{{{  virtual stuff

int CFolditView::CanClipCurLineP()
{
  if (IsHighlightedP())
    return FALSE;
  else
    return TRUE;
}

void CFolditView::ClipCurLine()
{

  int CurIndent   = GetCurIndent();
  int CurLength   = GetCurLength();
  int CurLine     = GetCurLine();
  int LineOffset  = GetEditCtrl().LineIndex (CurLine);

  POSITION pos = m_pCurrent->m_LineList.FindIndex(CurLine);
  CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

  pLine->m_Body.RemoveTrailingWhite();

  if (CurIndent > pLine->Render())
    pLine->m_Body += CString(' ', CurIndent - pLine->Render()); //pad to cursor pos

   //create an appropriate hint for the update
  CHintReplace *pReplace = new CHintReplace;
  MEM(pReplace);

  pReplace->m_pCurrent = m_pCurrent;
  pReplace->m_Chars    = "";
  pReplace->m_StartSel = LineOffset+pLine->Render();
  pReplace->m_EndSel   = LineOffset+CurLength;
  pReplace->m_Sel      = MAKELONG (pReplace->m_StartSel, pReplace->m_StartSel);
  pReplace->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

  m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);

  delete pReplace;
  m_pDoc->SetModifiedFlag();
}

int CFolditView::InVirtualSpace() //not used
{
  int CurIndent   = GetCurIndent();
  int CurLine     = GetCurLine();

  POSITION pos = m_pCurrent->m_LineList.FindIndex(CurLine);
  CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

  if (CurIndent > (pLine->GetReadWriteIndent()+pLine->m_Body.FindTrailingWhite()))
    return TRUE;
  else
    return FALSE;
}

//}}}
//{{{  ExtendRight

//{{{  CanP

BOOL CFolditView::CanExtendRightP ()
{
    POSITION pos = FALSE;

    if (IsHighlightedP())
        return FALSE;

    int CurIndent   = GetCurIndent();
    int CurLength   = GetCurLength();

    if (CurIndent == CurLength)
    {
        //MessageBeep(-1);
        pos = m_pCurrent->CanInsertCharAtP (GetCurLine(), CurIndent, ' ');
    }

    if (pos)
      return TRUE;
    else
      return FALSE;
}

//}}}
//{{{  Do

void CFolditView::DoExtendRight ()
{
    if (CanExtendRightP())
    {
        App()->m_KeyMacroPlayBack = TRUE;  //hack to force proper update
        DoInsertChar (' ');
        App()->m_KeyMacroPlayBack = FALSE;
    }
}

//}}}

//}}}
//{{{  SplitLine

//{{{  CanP

BOOL CFolditView::CanSplitLineP ()
{
    if (IsHighlightedP())
        return CanEditDeleteP();

    int nLine = GetCurLine();
    int nChar = GetCurIndent();

    return m_pCurrent->CanSplitLineAtP (nLine, nChar);
}

//}}}
//{{{  Do

void CFolditView::DoSplitLine ()
{
       if (IsHighlightedP())
    DoEditDelete();

       if (!CanSplitLineP())
      return;

      int nLine = GetCurLine();
      int nChar = GetCurIndent();

      m_pCurrent->SplitLineAt (nLine, nChar);
        //line split OK

        //doc has changed
        m_pDoc->SetModifiedFlag();

        //create an appropriate hint for the update
        CHintReplace *pReplace = new CHintReplace;
        MEM(pReplace);

        pReplace->m_pCurrent    = m_pCurrent;
        pReplace->m_Chars               = "\r\n";
        pReplace->m_TopLine     = GetEditCtrl().GetFirstVisibleLine();

        GetEditCtrl().GetSel (pReplace->m_StartSel, pReplace->m_EndSel);

        //new sel will need to be 2 chars (CR+LF above) further on
        int nSel = pReplace->m_StartSel + pReplace->m_Chars.GetLength();

        pReplace->m_Sel = MAKELONG (nSel, nSel);

        //redraw all the views
        m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);

        delete pReplace;
}

//}}}

//}}}
//{{{  EditDeleteline

//{{{  CanP

BOOL CFolditView::CanEditDeletelineP()
{
    return m_pCurrent->CanDeleteLineAtP (GetCurLine()); // && !IsHighlightedP();
}

//}}}
//{{{  Do

void CFolditView::DoEditDeleteline()
{
    if (!CanEditDeletelineP())
        return;

    m_pCurrent->DeleteLineAt (GetCurLine());

        //view issue - dont allow an empty fold
          if (m_pCurrent->m_LineList.IsEmpty())
          m_pCurrent->m_LineList.AddTail (new CTextLine);

        //HACK experiment with this re caret position after deletion

        //deleted OK
        m_pDoc->SetModifiedFlag();

        CHintBlind *pHint = new CHintBlind;
        MEM(pHint);

        pHint->m_pCurrent = m_pCurrent;
        pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

        //force selection to first char on line (HACK, do more elegantly)
        int nChar = GetEditCtrl().LineIndex();
        pHint->m_Sel = MAKELONG (nChar, nChar);

        m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);

        delete pHint;
}

//}}}

//}}}
//{{{  EditCutline

//{{{  CanP

BOOL CFolditView::CanEditCutlineP()
{
    if (!CanEditCopylineP())
        return FALSE;

    if (!CanEditDeletelineP())
        return FALSE;

    return TRUE;
}

//}}}
//{{{  Do

void CFolditView::DoEditCutline()
{
      if (!CanEditCutlineP())
         return;

      DoEditCopyline();
      DoEditDeleteline();
}

//}}}

//}}}
//{{{  EditCopyline

//{{{  CanP

BOOL CFolditView::CanEditCopylineP()
{
  return m_pCurrent->CanDeleteLineAtP (GetCurLine()); // && !IsHighlightedP();
}

//}}}
//{{{  Do

void CFolditView::DoEditCopyline()
{
    if (!CanEditCopylineP())
        return;

    CLine *pLine = m_pCurrent->GetLineAt(GetCurLine());
    if (!pLine) return;

    CFoldLine *pPaste = App()->NewPaste("Cut/Copy Line");

    if (pLine->IsFoldP())
    {
        CFoldLine *pNewFold = new CFoldLine (*((CFoldLine *)pLine));
        pNewFold->m_pParent = pPaste;
        pPaste->m_LineList.AddTail(pNewFold);
    }
    else
    {
        App()->m_pPasteFold->m_LineList.AddTail(new CTextLine (pLine->m_Body));
    }

    //App()->CopyToClipboard(m_pDoc->m_pLanguage);
}

//}}}

//}}}
//{{{  EditPasteline

//{{{  CanP

BOOL CFolditView::CanEditPastelineP() //not used
{
     return CanEditPasteP();
     ////////////////////////////////////////

     if (IsHighlightedP())
        return FALSE;

     if (!App()->m_pPasteFold)
         return FALSE;

     if (!(App()->m_pPasteFold->m_LineList.GetCount()==1))
         return FALSE;

     return TRUE;
}

//}}}
//{{{  Do

void CFolditView::DoEditPasteline()
{
    /////////////////////////////////////////

    if (!CanEditPastelineP())
        return;

    if (App()->m_pPasteFold->m_LineList.GetCount() != 1)
    {
      DoEditAppend();
      return;
    }

    //it's a single line so add a CRLF to it - normal paste logic
    //will insert it without a CRLF

    POSITION pos = m_pCurrent->m_LineList.FindIndex(GetCurLine());
    ASSERT (pos);

    CLine *pLine = (CLine *)(App()->m_pPasteFold->m_LineList.GetAt (App()->m_pPasteFold->m_LineList.GetHeadPosition()));
    ASSERT (pLine);

    if (pLine->IsFoldP())
    {
    CFoldLine *pFold = new CFoldLine (*((CFoldLine *)pLine));
    MEM(pFold);

    pFold->m_pParent = m_pCurrent;
    m_pCurrent->m_LineList.InsertBefore (pos, pFold);
    }
    else
    {
    m_pCurrent->m_LineList.InsertBefore (pos, new CTextLine (pLine->m_Body));
    }

    m_pDoc->SetModifiedFlag();

    CHintBlind *pHint = new CHintBlind;
    MEM(pHint);

    pHint->m_pCurrent = m_pCurrent;
    pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

    //force selection to first char on line (HACK, do more elegantly)
    int nChar = GetEditCtrl().LineIndex();
    pHint->m_Sel = MAKELONG (nChar, nChar);

    m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);

    delete pHint;
}

//}}}

//}}}
//{{{  CutCopyDel

//{{{  CanP

BOOL CFolditView::CanCutCopyDelP (BOOL DoCut, BOOL DoCopy)
{
    int StartLine, EndLine, StartChar, EndChar;

    if (GetSelInfo (StartLine, StartChar, EndLine, EndChar))

        return m_pCurrent->CanCutCopyDelAtP (StartLine, StartChar, EndLine, EndChar, DoCut, DoCopy);

    return FALSE;
}

//}}}
//{{{  Do

void CFolditView::DoCutCopyDel (BOOL DoCut, BOOL DoCopy, BOOL CopyToClipboard)
{
    if (!CanCutCopyDelP(DoCut, DoCopy))
        return;

    int StartLine, EndLine, StartChar, EndChar;

    GetSelInfo (StartLine, StartChar, EndLine, EndChar);

    m_pCurrent->CutCopyDelAt (StartLine, StartChar, EndLine, EndChar, DoCut, DoCopy, m_pDoc->m_pLanguage, CopyToClipboard);

    if (DoCopy && !DoCut)
    {
        GetEditCtrl().GetSel (StartChar, EndChar);
        GetEditCtrl().SetSel (EndChar, EndChar);
    }

    if (DoCut)
    {
        //view issue - dont allow empty doc
        if (m_pCurrent->m_LineList.IsEmpty())
              m_pCurrent->m_LineList.AddTail (new CTextLine);

        //doc has changed
        m_pDoc->SetModifiedFlag();

        //update views
        CHintBlind *pHint = new CHintBlind;
        MEM(pHint);

        pHint->m_pCurrent = m_pCurrent;
        pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();
        pHint->m_Sel      = GetEditCtrl().GetSel();

        //force end sel to be equal to start sel for update
        pHint->m_Sel      = MAKELONG(LOWORD(pHint->m_Sel), LOWORD(pHint->m_Sel));

        m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);

        delete pHint;
    }
}

//}}}

//}}}
//{{{  EditCut

//{{{  CanP

BOOL CFolditView::CanEditCutP()
{
    return CanCutCopyDelP (TRUE, TRUE);
}

//}}}
//{{{  Do

void CFolditView::DoEditCut()
{
    DoCutCopyDel (TRUE, TRUE);
}

//}}}

//}}}
//{{{  EditCopy

//{{{  CanP

BOOL CFolditView::CanEditCopyP()
{
    return CanCutCopyDelP (FALSE, TRUE);
}

//}}}
//{{{  Do

void CFolditView::DoEditCopy(BOOL Copytoclipboard)
{
    DoCutCopyDel (FALSE, TRUE, Copytoclipboard);
}

//}}}

//}}}
//{{{  EditDelete

//{{{  CanP

BOOL CFolditView::CanEditDeleteP()
{
    return CanCutCopyDelP (TRUE, TRUE);
}

//}}}
//{{{  Do

void CFolditView::DoEditDelete()
{
    DoCutCopyDel (TRUE, TRUE, FALSE);
}

//}}}

//}}}
//{{{  EditPaste

//{{{  CanP

BOOL CFolditView::CanEditPasteP (BOOL Includeclipboard)
{
    BOOL IsClip;

    if (Includeclipboard)
      IsClip = App()->PasteFromClipboard(NULL);
    else
      IsClip = FALSE;

    if (!App()->m_pPasteFold && !IsClip)
        return FALSE;

    if (!IsClip)
      if (App()->m_pPasteFold->m_LineList.IsEmpty())
         return FALSE;

    int StartLine, EndLine, StartChar, EndChar;

    if (GetSelInfo (StartLine, StartChar, EndLine, EndChar))
    {
        if (CanEditDeleteP()) //HACK this should be DeleteCharP - but its useful like this
                              //so we can paste to the start of a fold.  Need to fix when
                              //pasting properly.
                              //is this good enough - highlight end fold title chars etc
            return m_pCurrent->CanPasteAtP (StartLine, StartChar);
        else
            return FALSE;
    }

    return m_pCurrent->CanPasteAtP (StartLine, StartChar);
}

//}}}
//{{{  Do

void CFolditView::DoEditAppend()
{
  int CurLine     = GetCurLine();
  int LineOffset  = GetEditCtrl().LineIndex (CurLine);

  GetEditCtrl().SetSel (LineOffset,LineOffset);
  DoEditPaste();
}

void CFolditView::DoEditPaste(BOOL Pastefromclipboard)
{
    if (!CanEditPasteP(Pastefromclipboard))
        return;

    if (IsHighlightedP())
        OnEditDelete ();

    ASSERT (!IsHighlightedP());

    if (!CanEditPasteP(Pastefromclipboard))
    {
        ImplementationRestriction ("Cant paste here after delete");
        return;
    }

    int StartLine, EndLine, StartChar, EndChar;

    GetSelInfo (StartLine, StartChar, EndLine, EndChar);

    if (Pastefromclipboard)
    {
       if (App()->PasteFromClipboard(NULL))
         App()->PasteFromClipboard(m_pDoc->m_pLanguage);
    }

    //now do the paste
    m_pCurrent->PasteAt (StartLine, StartChar);

    //will never get empty doc after paste

    //doc has changed
    m_pDoc->SetModifiedFlag();

    //update views
    CHintBlind *pHint = new CHintBlind;
    MEM(pHint);

    pHint->m_pCurrent = m_pCurrent;
    pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();
    pHint->m_Sel      = GetEditCtrl().GetSel();

    //force end sel to be equal to start sel for update
    pHint->m_Sel      = MAKELONG(LOWORD(pHint->m_Sel), LOWORD(pHint->m_Sel));

    m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);

    delete pHint;
}

//}}}

//}}}
//{{{  ExtendUpDown

//{{{  CanP

BOOL CFolditView::CanExtendUpDownP (int Dirn)
{
    if (IsHighlightedP())
        return FALSE;

    int CurIndent   = GetCurIndent();
    int CurLine     = GetCurLine();

    POSITION pos = m_pCurrent->m_LineList.FindIndex(CurLine);
    CLine *pLine;

    if (Dirn == 1)
        pLine = (CLine *)m_pCurrent->m_LineList.GetNext(pos);
    else
        pLine = (CLine *)m_pCurrent->m_LineList.GetPrev(pos);

    if (!pos)
        //no next/prev line
        return FALSE;

    pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

    if (pLine->Render() < CurIndent)
        return TRUE;

    return FALSE;
}

//}}}
//{{{  Do

void CFolditView::DoExtendUpDown (int Dirn)
{
    if (!CanExtendUpDownP(Dirn))
        return;

    int CurIndent   = GetCurIndent();
    int CurLine     = GetCurLine();

    POSITION pos = m_pCurrent->m_LineList.FindIndex(CurLine+Dirn);
    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

    int OldLineLen = pLine->Render();

    CString Extra (' ', CurIndent - OldLineLen);
        pLine->m_Body += Extra;

    //create an appropriate hint for the update
    CHintReplace *pReplace = new CHintReplace;
    MEM(pReplace);

    pReplace->m_pCurrent    = m_pCurrent;
    pReplace->m_Chars       = Extra;

    int LineOffset = GetEditCtrl().LineIndex (CurLine + Dirn);

    pReplace->m_StartSel = LineOffset + OldLineLen;
    pReplace->m_EndSel   = LineOffset + OldLineLen;
    pReplace->m_Sel      = MAKELONG(LineOffset+CurIndent,LineOffset+CurIndent);//GetEditCtrl().GetSel();
    pReplace->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

    //update local edit box with char
    //CEditView::OnChar (nChar, 1, 0);   //HACK flags??

    //redraw non-local views
    m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);

    delete pReplace;
}

//}}}

//}}}

//}}}
//{{{  DevTest

//{{{  DisplaySelectedParameters

void CFolditView::OnDebugDisplayselectedtextparameters()
{
    int a, b, c, d;

    GetSelInfo (a, b, c, d);
    TRACE ("%d, %d, %d, %d\n", a, b, c, d);
}

void CFolditView::OnUpdateDebugDisplayselectedtextparameters(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}
//}}}
//{{{  Display PasteBuffer

void CFolditView::OnDevtestShoeinternalpastebuffer()
{
    ASSERT (App()->m_pPasteFold);

    CFolditMDIChildWnd* pActiveChild = (CFolditMDIChildWnd *)GetParent(); //MDIGetActive();
    CDocument* pDocument;
    if (pActiveChild == NULL ||
      (pDocument = pActiveChild->GetActiveDocument()) == NULL)
    {
        TRACE0("Warning: No active document for WindowNew command\n");
        AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
        return;     // command failed
    }

    //otherwise we have a new frame !
    CDocTemplate* pTemplate = pDocument->GetDocTemplate();
    ASSERT_VALID(pTemplate);
    MEM(pTemplate);

    CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
    if (pFrame == NULL)
    {
        TRACE0("Warning: failed to create new frame\n");
        return;     // command failed
    }

    //lifted out of InitialUpdateFrame(), called below
    //if the frame does not have an active view, set to first pane
    if (pFrame->GetActiveView() == NULL)
    {
        CWnd* pWnd = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST);
        if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
        {
            //set the new view as active
            pFrame->SetActiveView((CView*)pWnd);

            CFolditView *pView = (CFolditView *)pWnd;
            pView->m_pCurrent = &(App()->m_PasteStack);
        }
    }

    pTemplate->InitialUpdateFrame(pFrame, pDocument);
}

void CFolditView::OnUpdateDevtestShoeinternalpastebuffer(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (App()->m_pPasteFold != NULL);
}
//}}}

//}}}
//{{{  handlers

//{{{  status line handlers

//{{{  OnUpdateIndicatorDirty
void CFolditView::OnUpdateIndicatorDirty(CCmdUI* pCmdUI)
{
    if (m_pDoc->IsModified())
      pCmdUI->SetText("Dirty");
    else
      pCmdUI->SetText("Clean");
    pCmdUI->Enable(TRUE);
}
//}}}
//{{{  OnUpdateIndicatorLast
void CFolditView::OnUpdateIndicatorLast(CCmdUI* pCmdUI)
{
    if (m_pDoc->GetPathName().IsEmpty())
      pCmdUI->SetText("");
    else
      pCmdUI->SetText(m_pDoc->m_FileStatus.m_mtime.Format("Last modified: %d/%m/%y %X"));
    pCmdUI->Enable(TRUE);
}
//}}}
//{{{  OnUpdateIndicatorWho
void CFolditView::OnUpdateIndicatorWho(CCmdUI* pCmdUI)
{
    pCmdUI->SetText(App()->m_DateTag);
    pCmdUI->Enable(TRUE);
}
//}}}
//{{{  OnUpdateIndicatorRed
void CFolditView::OnUpdateIndicatorRed(CCmdUI* pCmdUI)
{
    pCmdUI->SetText(m_pDoc->m_Folder);
    pCmdUI->Enable(TRUE);
}
//}}}
//{{{  OnUpdateIncicatorLine

void CFolditView::OnUpdateIndicatorLine(CCmdUI* pCmdUI)
{
    char pLine[18];

    _itoa (GetCurLine()+1, pLine, 10);

    pCmdUI->SetText(pLine);
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  OnUpdateIndicatorCol

void CFolditView::OnUpdateIndicatorCol(CCmdUI* pCmdUI)
{
    char pLine[18];

    _itoa (GetCurIndent()+1, pLine, 10);

    pCmdUI->SetText(pLine);
    pCmdUI->Enable(TRUE);
}

//}}}
//}}}
//{{{  keyboard handlers

//{{{  OnKeyDown

void CFolditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (App()->m_KeyMacroActive)
    {
        App()->m_KeyMacroRep.Add (nRepCnt);
        App()->m_KeyMacroChar.Add (nChar);
        App()->m_KeyMacroFunc.Add (1);
        App()->m_KeyMacroFlags.Add (nFlags);
    }
    TRACE2 ("on key down key=%x, flags=%x\n", nChar, nFlags);
    switch (nChar)
    {
        //{{{  delete
        
        case VK_DELETE:
        {
            DoDeleteChar();
            break;
        }
        
        //}}}
        //{{{  back
        
        case VK_BACK:
        {
            DoBackSpaceChar();
            break;
        }
        
        //}}}
        //{{{  home
        
        case VK_HOME:
        {
            if (!IsKeyPadCharP(nFlags))
            {
                if (CanFoldEnterfoldP())
                {
                    DoFoldEnterfold();
                    break;
                }
                if (CanOpenRefP())
                {
                    DoOpenRef();
                    break;
                }
                if (CanEditTaskP())
                {
                    DoEditTask();
                    break;
                }
                break;
            }
        
            CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
            break;
        }
        
        //}}}
        //{{{  PgDn
        
        case VK_NEXT:
        {
            if (!IsKeyPadCharP(nFlags))
            {
                if (CanEditTaskP())
                {
                    DoEditTask();
                    break;
                }
            }
        
            CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
            break;
        }
        
        //}}}
        //{{{  PgUp
        
        case VK_PRIOR:
        {
        
            if (!IsKeyPadCharP(nFlags))
                if (CanFoldExitfoldP())
                    DoFoldExitfold();
                else
                    ;
            else
                CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
        
            break;
        }
        //}}}
        //{{{  Down
        case VK_DOWN:
        {
            //if (CanExtendUpDownP(1))  //HACK
            //    DoExtendUpDown(1);
            //else
                CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
            break;
        }
        //}}}
        //{{{  Up
        case VK_UP:
        {
            //if (CanExtendUpDownP(-1)) //HACK
            //    DoExtendUpDown(-1);
            //else
                CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
            break;
        }
        //}}}
        //{{{  Right
        case VK_RIGHT:
        {
            //if (CanExtendRightP())   //HACK
            //    DoExtendRight();
            //else
                CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
            break;
        }
        //}}}
        //{{{  end
        case VK_END:
        {
            if (!IsKeyPadCharP(nFlags))
            {
                if (CanFoldRipP())
                    DoFoldRip();
                break;
            }
            else
            {
                //if (CanClipCurLineP())  //HACK
                //   ClipCurLine();
                //else
                   CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
                break;
            }
        }
        
        //}}}
        //{{{  other nav keys
        
        case VK_SCROLL:
        case VK_NUMLOCK:
        case VK_INSERT:
        case VK_ESCAPE:
        case VK_LEFT:
        {
            //let the edit box handle these locally, other views are not affected
            CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
        
            break;
        }
        
        //}}}
        //{{{  else flush
        
        default:
        {
            //flush other keys
            break;
        }
        
        //}}}
    }
    //GetEditCtrl().InvalidateRect(NULL);
    //GetEditCtrl().UpdateWindow();
}

//}}}
//{{{  OnChar

void CFolditView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (App()->m_KeyMacroActive)
    {
        App()->m_KeyMacroRep.Add (nRepCnt);
        App()->m_KeyMacroChar.Add (nChar);
        App()->m_KeyMacroFunc.Add (2);
        App()->m_KeyMacroFlags.Add (nFlags);
    }
    TRACE2 ("on char key=%x, flags=%x\n", nChar, nFlags);
    switch (nChar)
    {
        //{{{  esc
        
        case VK_ESCAPE:
        {
            CEditView::OnChar (nChar, nRepCnt, nFlags);
            break;
        }
        
        //}}}
        //{{{  tab
        
        case VK_TAB:
        {
            DoInsertTab();
            break;
        }
        
        //}}}
        //{{{  return
        
        case VK_RETURN:
        {
            DoSplitLine();
            break;
        }
        
        //}}}
        //{{{  back
        
        case VK_BACK:
        {
            if (nRepCnt < 0)
              CEditView::OnChar (nChar, 1, nFlags);
        
            break;
        }
        
        //}}}
        //{{{  visible char
        
        default:
        {
            DoInsertChar(nChar);
            break;
        }
        
        //}}}
    }
    //GetEditCtrl().InvalidateRect(NULL);
    //GetEditCtrl().UpdateWindow();
}

//}}}
//{{{  OnKeyUp

void CFolditView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CEditView::OnKeyUp(nChar, nRepCnt, nFlags);
}

//}}}

//}}}
//{{{  mouse handlers

//{{{  OnLButtonDblClk

void CFolditView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (CanFoldEnterfoldP())
        DoFoldEnterfold();
    else
        if (CanOpenRefP())
            DoOpenRef();
}

//}}}
//{{{  OnRButtonDblClk

void CFolditView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
    DoFoldExitfold();
}

//}}}
//{{{  OnLButtonDown

void CFolditView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CEditView::OnLButtonDown(nFlags, point);
}

//}}}
//{{{  OnRButtonDown

void CFolditView::OnRButtonDown(UINT nFlags, CPoint point)
{
    //OnRButtonDblClk(nFlags, point);

    CEditView::OnRButtonDown(nFlags, point);
}

//}}}

//}}}
//{{{  menu/toolbar/accel handlers

//{{{  OptionsFont

//{{{  Update

void CFolditView::OnUpdateOptionsFont(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  On

void CFolditView::OnOptionsFont()
{
    CFontDialog dlg (&m_lf, CF_EFFECTS|CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT);

    dlg.m_cf.rgbColors = m_cf.rgbColors;

    //m_lf will be updated
    if (dlg.DoModal() == IDOK)
    {
        if (m_pFont)
            delete m_pFont;

        m_pFont = new CFont;
        MEM(m_pFont);

        if (m_pFont)
        {
          if (m_pFont->CreateFontIndirect(&m_lf))
          {
            GetEditCtrl().SetFont (m_pFont);
            m_cf = dlg.m_cf;
            App()->m_DefFrGnd          = m_cf.rgbColors;
            App()->m_DefaultFontName   = m_lf.lfFaceName;
            App()->m_DefaultFontHeight = m_lf.lfHeight;
          }
        }
    }
}

//}}}

//}}}
//{{{  FoldEnterfold

//{{{  Update

void CFolditView::OnUpdateFoldEnterfold(CCmdUI* pCmdUI)
{
    if (CanFoldEnterfoldP())
    {
    pCmdUI->SetText ("Open");
    pCmdUI->Enable(TRUE);
    return;
    }

    if (CanOpenRefP())
    {
    pCmdUI->SetText ("Open (Ref)");
    pCmdUI->Enable(TRUE);
    return;
    }

    pCmdUI->SetText("Open");
    pCmdUI->Enable(FALSE);
}

//}}}
//{{{  On

void CFolditView::OnFoldEnterfold()
{
    if (CanFoldEnterfoldP())
    DoFoldEnterfold();

    else
    DoOpenRef();
}

//}}}

//}}}
//{{{  FoldExitfold

//{{{  Update

void CFolditView::OnUpdateFoldExitfold(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (CanFoldExitfoldP());
}

//}}}
//{{{  On

void CFolditView::OnFoldExitfold()
{
    DoFoldExitfold();
}

//}}}

//}}}
//{{{  FoldEnterselectedfold

//{{{  Update

void CFolditView::OnUpdateFoldEnterselectedfold(CCmdUI* pCmdUI)
{
    //HACK
    pCmdUI->Enable(FALSE);
}

//}}}
//{{{  On

void CFolditView::OnFoldEnterselectedfold()
{
    // HACK, TODO
}

//}}}

//}}}
//{{{  FoldCreate

//{{{  Update

void CFolditView::OnUpdateFoldCreate(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanFoldCreateP());
}

//}}}
//{{{  On

void CFolditView::OnFoldCreate()
{
    DoFoldCreate();
}

//}}}

//}}}
//{{{  FoldRip

//{{{  Update

void CFolditView::OnUpdateFoldRip(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanFoldRipP());
}

//}}}
//{{{  On

void CFolditView::OnFoldRip()
{
    DoFoldRip();
}

//}}}

//}}}
//{{{  FoldUnFold

//{{{  Update

void CFolditView::OnUpdateFoldUnfold(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanFoldUnfoldP());
}

//}}}
//{{{  On

void CFolditView::OnFoldUnfold()
{
    DoFoldUnfold();
}

//}}}

//}}}
//{{{  EditDeleteline

//{{{  Update

void CFolditView::OnUpdateEditDeleteline(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanEditDeletelineP());
}

//}}}
//{{{  On

void CFolditView::OnEditDeleteline()
{
    DoEditDeleteline();
}

//}}}

//}}}
//{{{  OptionsEditing

//{{{  Update

void CFolditView::OnUpdateOptionsEditing(CCmdUI* pCmdUI)
{
    //can always access editing options
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  On

void CFolditView::OnOptionsEditing()
{
    // HACK TODO: Add your command handler code here
}

//}}}

//}}}
//{{{  EditCut

//{{{  Update

void CFolditView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanCutCopyDelP (TRUE, TRUE));
}

//}}}
//{{{  On

void CFolditView::OnEditCut()
{
    DoEditCut();
}

//}}}

//}}}
//{{{  EditCopy

//{{{  Update

void CFolditView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanCutCopyDelP (FALSE, TRUE));
}

//}}}
//{{{  On

void CFolditView::OnEditCopy()
{
    DoEditCopy();
}

//}}}

//}}}
//{{{  EditCopytoclipboard

void CFolditView::OnEditCopytoclipboard()
{
  DoEditCopy(TRUE);
}

void CFolditView::OnUpdateEditCopytoclipboard(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(CanCutCopyDelP (FALSE, TRUE));
}

//}}}
//{{{  EditDelete

//{{{  Update

void CFolditView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanCutCopyDelP (TRUE, FALSE));
}

//}}}
//{{{  On

void CFolditView::OnEditDelete()
{
    DoEditDelete();
}

//}}}

//}}}
//{{{  EditPaste

//{{{  Update

void CFolditView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (CanEditPasteP());
}

//}}}
//{{{  On

void CFolditView::OnEditPaste()
{
    DoEditPaste();
}

//}}}

//}}}
//{{{  EditPastefromclipboard

void CFolditView::OnEditPastefromclipboard()
{
  DoEditPaste(TRUE);
}

void CFolditView::OnUpdateEditPastefromclipboard(CCmdUI* pCmdUI)
{
  pCmdUI->Enable (CanEditPasteP(TRUE));
}

//}}}
//{{{  EditUndo

//{{{  Update

//void CFolditView::OnUpdateEditUndo(CCmdUI* pCmdUI)
//{
//    ;
//    //pCmdUI->Enable (App()->mPasteStack.m_LineList.GetCount() != 0);
//}

//}}}
//{{{  Do

//void CFolditView::OnEditUndo()
//{
//    ;
//}

//}}}

//}}}
//{{{  ContextHelp

void CFolditView::OnContextHelp()
{
    NotImplementedYet();
}

//}}}
//{{{  FileOpenReference

//{{{  Update

void CFolditView::OnUpdateFileOpenreference(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanOpenRefP());
}

//}}}
//{{{  On

void CFolditView::OnFileOpenreference()
{
    if (CanOpenRefP())
        DoOpenRef();
}

//}}}

//}}}
//{{{  ToolsEditTask

//{{{  Update

void CFolditView::OnUpdateToolsedittask(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanEditTaskP());
}

//}}}
//{{{  On

void CFolditView::OnToolsedittask()
{
    if (CanEditTaskP())
        DoEditTask();
}

//}}}

//}}}
//{{{  EditXline
void CFolditView::OnEditCutline()
{
    DoEditCutline();
}

void CFolditView::OnUpdateEditCutline(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanEditCutlineP());
}

void CFolditView::OnEditCopyline()
{
    DoEditCopyline();
}

void CFolditView::OnUpdateEditCopyline(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (CanEditCopylineP());
}

void CFolditView::OnEditPasteline()
{
    DoEditPasteline();
}

void CFolditView::OnUpdateEditPasteline(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CanEditPastelineP());
}
//}}}
//{{{  ToolsTextToFold

//{{{  update

void CFolditView::OnUpdateToolsTexttofold(CCmdUI* pCmdUI)
{
    if (IsHighlightedP())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    POSITION pos = m_pCurrent->m_LineList.FindIndex(GetCurLine());
    if (pos == NULL)
    {
      pCmdUI->Enable(FALSE);
      return;
    }

    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);
    ASSERT (pLine);

    if (pLine->IsTextP())
        pCmdUI->Enable(TRUE);

    else
        pCmdUI->Enable(FALSE);
}

//}}}
//{{{  on

void CFolditView::OnToolsTexttofold()
{
    POSITION pos = m_pCurrent->m_LineList.FindIndex(GetCurLine());
    ASSERT (pos);

    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);
    ASSERT (pLine);

    if (pLine->IsTextP())
    {
        CFoldLine *pFold = new CFoldLine;
        MEM(pFold);
        pFold->m_pParent = m_pCurrent;
        pFold->m_Indent = pLine->m_Body.GetIndent();
        pFold->m_Body = pLine->m_Body;
        delete pLine;
        m_pCurrent->m_LineList.SetAt(pos, pFold);

        //doc has changed
        m_pDoc->SetModifiedFlag();

        //update views
        CHintBlind *pHint = new CHintBlind;
        MEM(pHint);

        pHint->m_pCurrent = m_pCurrent;
        pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();
        pHint->m_Sel      = GetEditCtrl().GetSel();

        //force end sel to be equal to start sel for update
        pHint->m_Sel      = MAKELONG(LOWORD(pHint->m_Sel), LOWORD(pHint->m_Sel));

        m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);

        delete pHint;
    }
}

//}}}
//}}}
//{{{  Find*

//{{{  Update Findfirst

void CFolditView::OnUpdateEditFindfirst(CCmdUI* pCmdUI)
{
  ((CMainFrame *)(App()->m_pMainWnd))->m_wndFindBox.GetWindowText(App()->m_SearchStr);
  pCmdUI->Enable(!App()->m_SearchStr.IsEmpty());
}

//}}}
//{{{  Update Findnext

void CFolditView::OnUpdateEditFindnext(CCmdUI* pCmdUI)
{
  ((CMainFrame *)(App()->m_pMainWnd))->m_wndFindBox.GetWindowText(App()->m_SearchStr);
  pCmdUI->Enable(!App()->m_SearchStr.IsEmpty() && App()->m_pLastFind);
}

//}}}
//{{{  On Findfirst

void CFolditView::OnEditFindfirst()
{
    int StartLine,StartChar,EndLine,EndChar;
    int Line, Char;
    CFoldLine *pNew = NULL;
    CFolditView *pView;
    int ss,es;
    int CharIndent,LineIndent;

    CString s;
    s=App()->m_SearchStr;
    s.MakeLower();

    ((CMainFrame *)(App()->m_pMainWnd))->m_pFocus = NULL;
    GetEditCtrl().SetFocus();
    ((CMainFrame *)(App()->m_pMainWnd))->UpdateFindEnv();

    if (!GetSelInfo(StartLine,StartChar,EndLine,EndChar))
    {
       CharIndent = GetCurIndent();
       LineIndent = GetCurLine();
    }
    else
    {
       CharIndent = EndChar+1;
       LineIndent = EndLine;
    }

    App()->m_pLastFind = m_pCurrent; //mark the top level fold limit

    if (m_pCurrent->Find (TRUE, s, LineIndent, CharIndent, &pNew, Line, Char))
    {
      if (pNew == m_pCurrent)
      {
        //{{{  move caret in this fold (returns)
        
        ss = GetEditCtrl().LineIndex (Line) + Char;
        es = ss + App()->m_SearchStr.GetLength();
        GetEditCtrl().SetSel (ss, es);
        return;
        //}}}
      }
      //{{{  create new child frame
      
      CFolditMDIChildWnd* pActiveChild = (CFolditMDIChildWnd *)GetParent();
      ASSERT (pActiveChild);
      MEM (pActiveChild);
      
      CDocument *pDocument = pActiveChild->GetActiveDocument();
      ASSERT (pDocument);
      MEM (pDocument);
      
      CDocTemplate* pTemplate = pDocument->GetDocTemplate();
      ASSERT_VALID(pTemplate);
      MEM(pTemplate);
      
      CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
      ASSERT (pFrame);
      MEM (pFrame);
      
      if (pFrame->GetActiveView() == NULL)
      {
        CWnd* pWnd = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST);
      
        if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
        {
          //set the new view as active
          pFrame->SetActiveView((CView*)pWnd);
      
          pView = (CFolditView *)pWnd;
          pView->m_pCurrent = pNew;
      
          //Check that it has at least one line in it - backwards compat with
          //older versions
          if (pView->m_pCurrent->m_LineList.IsEmpty())
              pView->m_pCurrent->m_LineList.AddTail (new CTextLine);
        }
      }
      //}}}
      //{{{  and display it with word as caret (returns)
      
      pTemplate->InitialUpdateFrame(pFrame, pDocument);
      //20/11/99 DCN HACK ss,es not initialised at this point, so removed-->>pView->GetEditCtrl().SetSel(ss,es);
      //pView->m_SearchStr = m_SearchStr;
      pView->GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + pView->m_pCurrent->m_Body);
      ss = pView->GetEditCtrl().LineIndex (Line) + Char;
      es = ss + App()->m_SearchStr.GetLength();
      pView->GetEditCtrl().SetSel (ss, es);
      
      return;
      //}}}
    }
    else
    {
      MessageBeep(0xFFFFFFFF);
      AfxMessageBox("["+App()->m_SearchStr+"] not found in this fold or it's children");
    }
}

//}}}
//{{{  On Findnext

void CFolditView::OnEditFindnext()
{
    if (!m_pDoc->m_TopFold.IsSubFoldP(App()->m_pLastFind))
    {
      MessageBeep(0xFFFFFFFF);
      AfxMessageBox("The fold that was marking FindFirst has been removed or is in another document; do FindFirst again to reset the context.");
      return;
    }

    int StartLine,StartChar,EndLine,EndChar;
    int Line, Char;
    CFoldLine *pNew = NULL;
    CFolditView *pView;
    int ss,es;
    int CharIndent,LineIndent;

    CString s;
    s=App()->m_SearchStr;
    s.MakeLower();

    ((CMainFrame *)(App()->m_pMainWnd))->m_pFocus = NULL;
    GetEditCtrl().SetFocus();
    ((CMainFrame *)(App()->m_pMainWnd))->UpdateFindEnv();

    if (!GetSelInfo(StartLine,StartChar,EndLine,EndChar))
    {
       CharIndent = GetCurIndent();
       LineIndent = GetCurLine();
    }
    else
    {
       CharIndent = EndChar+1;
       LineIndent = EndLine;
    }

    if (m_pCurrent->Find (FALSE, s, LineIndent, CharIndent, &pNew, Line, Char))
    {
      if (pNew == m_pCurrent)
      {
        //{{{  move caret in this fold (returns)
        
        ss = GetEditCtrl().LineIndex (Line) + Char;
        es = ss + App()->m_SearchStr.GetLength();
        GetEditCtrl().SetSel (ss, es);
        return;
        //}}}
      }
      //{{{  create new child frame
      
      CFolditMDIChildWnd* pActiveChild = (CFolditMDIChildWnd *)GetParent();
      ASSERT (pActiveChild);
      MEM (pActiveChild);
      
      CDocument *pDocument = pActiveChild->GetActiveDocument();
      ASSERT (pDocument);
      MEM (pDocument);
      
      CDocTemplate* pTemplate = pDocument->GetDocTemplate();
      ASSERT_VALID(pTemplate);
      MEM(pTemplate);
      
      CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
      ASSERT (pFrame);
      MEM (pFrame);
      
      if (pFrame->GetActiveView() == NULL)
      {
        CWnd* pWnd = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST);
      
        if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
        {
          //set the new view as active
          pFrame->SetActiveView((CView*)pWnd);
      
          pView = (CFolditView *)pWnd;
          pView->m_pCurrent = pNew;
      
          //Check that it has at least one line in it - backwards compat with
          //older versions
          if (pView->m_pCurrent->m_LineList.IsEmpty())
              pView->m_pCurrent->m_LineList.AddTail (new CTextLine);
        }
      }
      //}}}
      //{{{  and display it with word as caret (returns)
      
      pTemplate->InitialUpdateFrame(pFrame, pDocument);
      // 20/11/99 DCN HACK removed 'cos ss,es not setup yet-->>pView->GetEditCtrl().SetSel (ss, es);
      //pView->m_SearchStr = m_SearchStr;
      pView->GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + pView->m_pCurrent->m_Body);
      ss = pView->GetEditCtrl().LineIndex (Line) + Char;
      es = ss + App()->m_SearchStr.GetLength();
      pView->GetEditCtrl().SetSel (ss, es);
      
      return;
      //}}}
    }
    else
    {
      MessageBeep(0xFFFFFFFF);
      AfxMessageBox("["+App()->m_SearchStr+"] not found in context of fold:-\n\n"+\
                    "...  "+App()->m_pLastFind->m_Body);
    }
}

//}}}
//}}}
//{{{  Searchforinitial

void CFolditView::Searchforinitial()
{
    int StartLine,StartChar,EndLine,EndChar;
    int Line, Char;
    CFoldLine *pNew = NULL;
    CFolditView *pView;
    int ss,es;
    int CharIndent,LineIndent;

    CString s;
    s=App()->m_InitFind;
    s.MakeLower();

    if (App()->m_InitFind.IsEmpty())
      return;

    if (!GetSelInfo(StartLine,StartChar,EndLine,EndChar))
    {
       CharIndent = GetCurIndent();
       LineIndent = GetCurLine();
    }
    else
    {
       CharIndent = EndChar+1;
       LineIndent = EndLine;
    }

    App()->m_pLastFind = m_pCurrent;
    if (m_pCurrent->Find (TRUE, s, LineIndent, CharIndent, &pNew, Line, Char))
    {
      App()->m_pLastFind = NULL;
      if (pNew == m_pCurrent)
      {
        //{{{  move caret in this fold (returns)
        
        ss = GetEditCtrl().LineIndex (Line) + Char;
        es = ss + App()->m_InitFind.GetLength();
        //es = ss;
        GetEditCtrl().SetSel (ss, es);
        
        App()->m_InitFind = "";
        return;
        //}}}
      }
      //{{{  create new child frame
      
      CFolditMDIChildWnd* pActiveChild = (CFolditMDIChildWnd *)GetParent();
      ASSERT (pActiveChild);
      MEM (pActiveChild);
      
      CDocument *pDocument = pActiveChild->GetActiveDocument();
      ASSERT (pDocument);
      MEM (pDocument);
      
      CDocTemplate* pTemplate = pDocument->GetDocTemplate();
      ASSERT_VALID(pTemplate);
      MEM(pTemplate);
      
      CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
      ASSERT (pFrame);
      MEM (pFrame);
      
      if (pFrame->GetActiveView() == NULL)
      {
        CWnd* pWnd = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST);
      
        if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
        {
          //set the new view as active
          pFrame->SetActiveView((CView*)pWnd);
      
          pView = (CFolditView *)pWnd;
          pView->m_pCurrent = pNew;
      
          //Check that it has at least one line in it - backwards compat with
          //older versions
          if (pView->m_pCurrent->m_LineList.IsEmpty())
              pView->m_pCurrent->m_LineList.AddTail (new CTextLine);
        }
      }
      //}}}
      //{{{  and display it with word as caret (returns)
      
      pTemplate->InitialUpdateFrame(pFrame, pDocument);
      // 20/11/99 DCN HACK removed 'cos ss,es not setup yet-->>pView->GetEditCtrl().SetSel (ss, es);
      pView->GetParent()->SetWindowText (m_pDoc->GetTitle() + " :: " + pView->m_pCurrent->m_Body);
      ss = pView->GetEditCtrl().LineIndex (Line) + Char;
      //es = ss + App()->m_InitFind.GetLength();
      es = ss;
      pView->GetEditCtrl().SetSel (ss, es);
      
      App()->m_InitFind = "";
      return;
      //}}}
    }
    else
    {
      App()->m_InitFind  = "";
      App()->m_pLastFind = NULL;
    }
}

//}}}
//{{{  recorder stuff
void CFolditView::OnToolsStartkeyrecorder()
{
    App()->m_KeyMacroChar.RemoveAll();
    App()->m_KeyMacroFlags.RemoveAll();
    App()->m_KeyMacroFunc.RemoveAll();
    App()->m_KeyMacroRep.RemoveAll();
    App()->m_KeyMacroActive = TRUE;
}

void CFolditView::OnUpdateToolsStartkeyrecorder(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!App()->m_KeyMacroActive);
}

void CFolditView::OnUpdateToolsStopkeyrecorder(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(App()->m_KeyMacroActive);
}

void CFolditView::OnToolsStopkeyrecorder()
{
    App()->m_KeyMacroActive = FALSE;
}

void CFolditView::OnToolsPlaykeyrecording()
{
    int i;

    App()->m_KeyMacroPlayBack = TRUE;

    for (i=0; i<App()->m_KeyMacroChar.GetSize(); i++)
    {
      if (App()->m_KeyMacroFunc[i] == 1)
      {
        OnKeyDown (App()->m_KeyMacroChar[i], App()->m_KeyMacroRep[i], App()->m_KeyMacroFlags[i]);
      }
      else
      {
        OnChar    (App()->m_KeyMacroChar[i], App()->m_KeyMacroRep[i], App()->m_KeyMacroFlags[i]);
      }
    }

    App()->m_KeyMacroPlayBack = FALSE;

}

void CFolditView::OnUpdateToolsPlaykeyrecording(CCmdUI* pCmdUI)
{
    pCmdUI->Enable((!App()->m_KeyMacroActive) && (App()->m_KeyMacroChar.GetSize() > 0));
}

//}}}
//{{{  insert date
void CFolditView::OnToolsInsertdate()
{
    int i;
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;
    CString strDate;
    time_t globaltime;
    struct tm *loctime;
    char *pBuf;

    time(&globaltime);
    loctime=localtime(&globaltime);
    pBuf=strDate.GetBuffer(9);
    sprintf(pBuf,"%02d/%02d/%02d",loctime->tm_mday,loctime->tm_mon+1,loctime->tm_year-100);
    strDate.ReleaseBuffer();
    strDate = strDate + CString(' ',9-strDate.GetLength()) + App()->m_DateTag + " ";

    if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, ' ') || IsHL)
      return;

    ASSERT (!IsHL);

    int CurLength   = GetCurLength();
    int LineOffset  = GetEditCtrl().LineIndex (CurLine);

    pos = m_pCurrent->m_LineList.FindIndex(CurLine);
    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

    int Extra = m_pDoc->m_pLanguage->m_Tab - (CurIndent % m_pDoc->m_pLanguage->m_Tab);

    for (i=0; i<strDate.GetLength(); i++)
      pLine->InsertChar(CurIndent+i,strDate[i]);

    //create an appropriate hint for the update
    CHintReplace *pReplace = new CHintReplace;
    MEM(pReplace);

    pReplace->m_pCurrent = m_pCurrent;
    pReplace->m_Chars    = strDate;
    pReplace->m_StartSel = LineOffset+CurIndent;
    pReplace->m_EndSel   = LineOffset+CurIndent;
    pReplace->m_Sel      = MAKELONG (LineOffset+CurIndent+strDate.GetLength(), LineOffset+CurIndent+strDate.GetLength());
    pReplace->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

    m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);
    delete pReplace;
    m_pDoc->SetModifiedFlag();
}

void CFolditView::OnUpdateToolsInsertdate(CCmdUI* pCmdUI)
{
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;

    int ok=CanInsertCharP(IsHL, pos, CurLine, CurIndent, 'x');
    pCmdUI->Enable(ok && !IsHL);
}

//}}}
//{{{  insert title
void CFolditView::OnToolsInserttitle()
{
    int i;
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;
    CString strTitle;

    strTitle = m_pCurrent->m_Body;

    if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, ' ') || IsHL)
      return;

    ASSERT (!IsHL);

    int CurLength   = GetCurLength();
    int LineOffset  = GetEditCtrl().LineIndex (CurLine);

    pos = m_pCurrent->m_LineList.FindIndex(CurLine);
    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

    int Extra = m_pDoc->m_pLanguage->m_Tab - (CurIndent % m_pDoc->m_pLanguage->m_Tab);

    for (i=0; i<strTitle.GetLength(); i++)
      pLine->InsertChar(CurIndent+i,strTitle[i]);

    //create an appropriate hint for the update
    CHintReplace *pReplace = new CHintReplace;
    MEM(pReplace);

    pReplace->m_pCurrent = m_pCurrent;
    pReplace->m_Chars    = strTitle;
    pReplace->m_StartSel = LineOffset+CurIndent;
    pReplace->m_EndSel   = LineOffset+CurIndent;
    pReplace->m_Sel      = MAKELONG (LineOffset+CurIndent+strTitle.GetLength(), LineOffset+CurIndent+strTitle.GetLength());
    pReplace->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

    m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);
    delete pReplace;
    m_pDoc->SetModifiedFlag();
}

void CFolditView::OnUpdateToolsInserttitle(CCmdUI* pCmdUI)
{
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;

    int ok=CanInsertCharP(IsHL, pos, CurLine, CurIndent, 'x');
    pCmdUI->Enable(ok && !IsHL);
}

//}}}
//{{{  insert find-string
void CFolditView::OnToolsInsertfindstring()
{
    int i;
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;
    CString strTitle;

    strTitle = App()->m_SearchStr;

    if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, ' ') || IsHL)
      return;

    ASSERT (!IsHL);

    int CurLength   = GetCurLength();
    int LineOffset  = GetEditCtrl().LineIndex (CurLine);

    pos = m_pCurrent->m_LineList.FindIndex(CurLine);
    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

    int Extra = m_pDoc->m_pLanguage->m_Tab - (CurIndent % m_pDoc->m_pLanguage->m_Tab);

    for (i=0; i<strTitle.GetLength(); i++)
      pLine->InsertChar(CurIndent+i,strTitle[i]);

    //create an appropriate hint for the update
    CHintReplace *pReplace = new CHintReplace;
    MEM(pReplace);

    pReplace->m_pCurrent = m_pCurrent;
    pReplace->m_Chars    = strTitle;
    pReplace->m_StartSel = LineOffset+CurIndent;
    pReplace->m_EndSel   = LineOffset+CurIndent;
    pReplace->m_Sel      = MAKELONG (LineOffset+CurIndent+strTitle.GetLength(), LineOffset+CurIndent+strTitle.GetLength());
    pReplace->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

    m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);
    delete pReplace;
    m_pDoc->SetModifiedFlag();
}

void CFolditView::OnUpdateToolsInsertfindstring(CCmdUI* pCmdUI)
{
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;

    int ok=CanInsertCharP(IsHL, pos, CurLine, CurIndent, 'x');
    pCmdUI->Enable(ok && !IsHL);
}

//}}}
//{{{  insert serial no
void CFolditView::OnToolsInsertnextserialnumber()
{
    int i;
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;
    CString strSerialNo;

    char *pBuf = strSerialNo.GetBuffer(32);
    App()->m_SerialNo += 1;
    sprintf(pBuf,App()->m_SerialFmt,App()->m_SerialNo);
    strSerialNo.ReleaseBuffer();

    if (!CanInsertCharP(IsHL, pos, CurLine, CurIndent, ' ') || IsHL)
      return;

    ASSERT (!IsHL);

    int CurLength   = GetCurLength();
    int LineOffset  = GetEditCtrl().LineIndex (CurLine);

    pos = m_pCurrent->m_LineList.FindIndex(CurLine);
    CLine *pLine = (CLine *)m_pCurrent->m_LineList.GetAt(pos);

    int Extra = m_pDoc->m_pLanguage->m_Tab - (CurIndent % m_pDoc->m_pLanguage->m_Tab);

    for (i=0; i<strSerialNo.GetLength(); i++)
      pLine->InsertChar(CurIndent+i,strSerialNo[i]);

    //create an appropriate hint for the update
    CHintReplace *pReplace = new CHintReplace;
    MEM(pReplace);

    pReplace->m_pCurrent = m_pCurrent;
    pReplace->m_Chars    = strSerialNo;
    pReplace->m_StartSel = LineOffset+CurIndent;
    pReplace->m_EndSel   = LineOffset+CurIndent;
    pReplace->m_Sel      = MAKELONG (LineOffset+CurIndent+strSerialNo.GetLength(), LineOffset+CurIndent+strSerialNo.GetLength());
    pReplace->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();

    m_pDoc->UpdateAllViews (NULL, HINT_REPLACE, pReplace);
    delete pReplace;
    m_pDoc->SetModifiedFlag();
}

void CFolditView::OnUpdateToolsInsertnextserialnumber(CCmdUI* pCmdUI)
{
    BOOL IsHL;
    POSITION pos;
    int CurLine;
    int CurIndent;

    int ok=CanInsertCharP(IsHL, pos, CurLine, CurIndent, 'x');
    pCmdUI->Enable(ok && !IsHL);
}
//}}}
//{{{  indent all lines
void CFolditView::OnToolsIndentalllines()
{
    int StartLine,StartChar,EndLine,EndChar;
    int StartExtra,EndExtra,i;

    if (!GetSelInfo(StartLine,StartChar,EndLine,EndChar))
    {
      StartLine  = 0;
      EndLine    = m_pCurrent->m_LineList.GetCount() - 1;
      StartExtra = GetCurLine() + 1;  //nothing highlighted
      EndExtra   = StartExtra;        //so move caret one position right
    }
    else
    {
      StartExtra = 1;
      EndExtra   = EndLine-StartLine+1;
    }

    for (i=StartLine; i<=EndLine; i++)
      m_pCurrent->InsertCharAt(m_pCurrent->m_LineList.FindIndex(i),0,' ');

    CHintBlind *pHint = new CHintBlind;
    MEM(pHint);

    pHint->m_pCurrent = m_pCurrent;
    pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();
    pHint->m_Sel      = GetEditCtrl().GetSel();
    pHint->m_Sel      = MAKELONG(LOWORD(pHint->m_Sel)+StartExtra, HIWORD(pHint->m_Sel)+EndExtra);

    m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);
    m_pDoc->SetModifiedFlag();

    delete pHint;
}

void CFolditView::OnUpdateToolsIndentalllines(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

//}}}
//{{{  outdent all lines

void CFolditView::OnToolsOutdentalllines()
{
    int StartLine,StartChar,EndLine,EndChar;
    int StartExtra,EndExtra;

    if (!GetSelInfo(StartLine,StartChar,EndLine,EndChar))
    {
      StartLine  = 0;
      EndLine    = m_pCurrent->m_LineList.GetCount() - 1;
      StartExtra = GetCurLine() + 1;  //nothing highlighted
      EndExtra   = StartExtra;        //so move caret one position left
    }
    else
    {
      StartExtra = 1;
      EndExtra   = EndLine-StartLine+1;
    }

    m_pCurrent->Outdent(StartLine,EndLine);

    CHintBlind *pHint = new CHintBlind;
    MEM(pHint);

    pHint->m_pCurrent = m_pCurrent;
    pHint->m_TopLine  = GetEditCtrl().GetFirstVisibleLine();
    pHint->m_Sel      = GetEditCtrl().GetSel();
    pHint->m_Sel      = MAKELONG(LOWORD(pHint->m_Sel)-StartExtra, HIWORD(pHint->m_Sel)-EndExtra);

    m_pDoc->UpdateAllViews (NULL, HINT_BLIND, pHint);
    m_pDoc->SetModifiedFlag();

    delete pHint;
}

void CFolditView::OnUpdateToolsOutdentalllines(CCmdUI* pCmdUI)
{
    int StartLine,StartChar,EndLine,EndChar;

    if (!GetSelInfo(StartLine,StartChar,EndLine,EndChar))
    {
      StartLine = 0;
      EndLine   = m_pCurrent->m_LineList.GetCount() - 1;
    }

    pCmdUI->Enable(m_pCurrent->CanOutdentP(StartLine,EndLine));
}
//}}}

//}}}

//}}}

