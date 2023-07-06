
#ifndef __FOLDLINE_H__
#define __FOLDLINE_H__

//{{{  includes

#ifndef __LINE_H__
#include "line.h"
#endif

#ifndef __LINELIST_H__
#include "linelist.h"
#endif

#ifndef __LANGUAGE_H__
#include "language.h"
#endif

//}}}

//{{{  CFoldLine

class CFoldLine : public CLine
{
  //{{{  static
  public: //static data (one copy per class)
      static CString m_Prefix;
  //}}}
  //{{{  context
  public: //data
      int         m_Indent;
      CLineList   m_LineList;
      CFoldLine   *m_pParent;
      int         m_UsageCount;
  //}}}
  //{{{  constructors
  public:  //constructors
      CFoldLine (const char *Buf = NULL);
      CFoldLine (const CFoldLine &Source);
  //}}}
  //{{{  operators
  public: //operators
      CFoldLine &operator= (const CFoldLine &Source);
  //}}}
  //{{{  management interface
  public: //management interface
      virtual void    Acquire() {m_UsageCount++;};
      virtual void    Release() {m_UsageCount--;ASSERT(m_UsageCount >= 0);};
  //}}}
  //{{{  access interface
  public: //access interface
              BOOL    Find(BOOL Limit, CString &What, int FromLine, int FromChar, CFoldLine **Fold, int &Line, int &Char);
      virtual BOOL    IsFoldP() const {return TRUE;};
      virtual BOOL    IsEnterableP() const {return TRUE;};
      virtual BOOL    IsExitableP() const {return (m_pParent ? TRUE : FALSE);};
      virtual BOOL    IsEnterableAtP (const int nLine) const;
      virtual BOOL    IsActiveP() const {return (BOOL)m_UsageCount;};
      virtual BOOL    IsActiveKidsP() const;
      virtual BOOL    IsSubFoldP(CLine *pThis) const;
      virtual CString GetChars (const int StartChar, const int EndChar);
      virtual CString GetChars (const int StartChar);
      virtual CLine   *GetLineAt (const int nLine) const;
      virtual int     Render ();
      virtual void    Render (CString &Buf) const;
      virtual CString &GetPrefix () const {return m_Prefix;};
      virtual int     GetReadWriteIndent ();
      virtual int     GetTitleLength ();
      virtual BOOL    IsActiveAtP (int, int);
      virtual void    CopyFoldFromTo (const CFoldLine *pFrom, CFoldLine *pTo);
  //}}}
  //{{{  edit interface
  public: //edit interface
      virtual void WriteFoldsToString (CFoldLine *pFold, CString &UnfoldedStr, CLanguage *pLang);
      virtual void WriteFold (CFoldLine *pFold, int CurIndent, CString &UnfoldedStr, CLanguage *pLang);
  
      virtual void RemoveTrailing ();
  
      virtual BOOL CanDeleteCharP (const int nChar);
      virtual void DeleteChar     (const int nChar);
  
      virtual BOOL CanOutdentP (const int StartLine, const int EndLine);
      virtual void Outdent     (const int StartLine, const int EndLine);
  
      virtual BOOL CanDeleteCharsP (const int StartChar, const int EndChar);
      virtual void DeleteChars     (const int StartChar, const int EndChar);
  
      virtual BOOL CanDeleteCharsP (const int StartChar);
      virtual void DeleteChars     (const int StartChar);
  
      virtual BOOL CanInsertCharP (const int nChar, const UINT Char);
      virtual void InsertChar     (const int nChar, const UINT Char);
      virtual void InsertChars    (const int nChar, CString Chars);
  
      virtual BOOL CanCreateChildFoldAtP (const int nStart, const int nLen, const int nIndent);
      virtual void CreateChildFoldAt     (const int nStart, const int nLen, const int nIndent);
  
      virtual BOOL CanCreateTailFoldAtP (const int nIndent);
      virtual void CreateTailFoldAt     (const int nIndent);
  
      virtual BOOL CanCutCopyDelAtP (int, int, int, int, BOOL DoCut, BOOL DoCopy);
      virtual void CutCopyDelAt     (int, int, int, int, BOOL DoCut, BOOL DoCopy, CLanguage *pLang, BOOL CopyToClipboard);
  
      virtual BOOL CanPasteAtP (int, int);
      virtual void PasteAt     (int, int);
  
      virtual BOOL CanDeleteLineAtP (const int nLine);
      virtual void DeleteLineAt     (const int nLine);
  
      virtual BOOL CanUnFoldAtP (const int nLine);
      virtual void UnFoldAt     (const int nLine);
  
      virtual BOOL CanDeleteCharAtP (const int nLine, const int nChar);
      virtual void DeleteCharAt     (const int nLine, const int nChar);
  
      virtual POSITION CanInsertCharAtP (const int nLine, const int nChar, const UINT Char);
      virtual void     InsertCharAt     (POSITION pos, const int nChar, const UINT Char);
  
      virtual BOOL CanSplitLineAtP (const int nLine, const int nChar);
      virtual void SplitLineAt     (const int nLine, const int nChar);
  //}}}
};

//}}}

#endif

