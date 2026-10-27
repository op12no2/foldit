//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd

#ifndef __TOOLTIP_H
#define __TOOLTIP_H

class CPopupTip;

const char SZTIPINI_SECTION[] = "ToolTips";
const char SZTIPINI_FLYBY[] = "FlyBy";
const char SZTIPINI_TIPS[] = "Tips";
const char SZTIPINI_ROUND[] = "BoxStyle";
const char SZTIPINI_WAIT[] = "Wait";
const char SZTIPINI_DISPLAY[] = "Display";
const char SZTIPINI_FONT[] = "SystemFont";

#define TTIPS_SQUARESTYLE   0
#define TTIPS_ROUNDSTYLE    1
#define TTIPS_3DSTYLE     2
#define TTIPS_DEFAULT_TIME 500

//CToolTipBar
class CToolTipBar : public CToolBar
{
    DECLARE_DYNAMIC(CToolTipBar)
// Construction
public:
    CToolTipBar();
    virtual ~CToolTipBar();

private:
    CUIntArray m_pIDXCustom;  // Array of custom controls (e.g., COMBOBOX)
    BOOL DoIdle(LONG lCount);
    BOOL m_bFlyby;
    BOOL m_bTips;
    UINT m_nWait;
    UINT m_nStyleFlag;
    BOOL m_bToolbarDisplay;
    BOOL m_bSystemFont;

    BOOL m_bDirty;
    DWORD m_dwTicks;
    BOOL m_bIn;
    UINT m_uIdCache;
    CString m_strPopupClassName;
    CPopupTip *m_pPopup;
    CFont *m_pFont;
    BOOL m_bMousePressed;
    POINT GetBoxPosition(CRect ToolBarRect);
    void SetupFont ();
    void CleanDirty (int ClearCache);

public:
    UINT GetTipWait ();
    BOOL GetFlyby ();
    BOOL GetTips ();
    UINT GetTipStyle ();
    BOOL GetTipFont ();
    BOOL GetToolbarDisplay();
    void GetToolbarOptions();

    void SetTipWait (UINT bNewWait);
    void SetFlyby (BOOL bNewFlyby);
    void SetTips (BOOL bNewTips);
    void SetTipStyle (UINT bNewStyle);
    void SetTipFont (BOOL bSystemFont);
    void SetToolbarDisplay (BOOL bNewToolBarDisplay);
    void WriteToolbarOptions();
    void AddCustomButton(UINT nIDXControl);

protected:
    //{{AFX_MSG(CToolTipBar)
    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnSelEndOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
protected:
};

#endif   //__TOOLTIP_H

