#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
// CLBEdit

class CLBEdit : public REdit
{
	friend class RWindow;

protected:
	void OnKillFocus(HWND hWndGetFocus);
	LRESULT OnGetDlgCode(UINT virtKey, MSG *pMsg);
	void OnKeyDown(UINT virtKey, WORD repCount, UINT flags);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// CEditListBox

class CEditListBox : public RWindow
{
	friend class RWindow;

public:
	CEditListBox();
	bool Create(HWND hWndParent, bool bShowSelAlways = false, const TCHAR *lpszEditBtnText = NULL);
	INT_PTR Add(const TCHAR *lpszText);
	bool SetSel(INT_PTR nIndex);
	INT_PTR GetSel() const;
	bool Edit(INT_PTR nIndex);
	CLBEdit* GetEditControl();
	
protected:
	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	bool OnCreate(CREATESTRUCT *pCS);
	void OnSize(DWORD type, WORD cx, WORD cy);
	void OnPaint(HDC hDC);
	void OnHScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar);
	void OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar);
	void OnLButtonDown(DWORD keys, short x, short y);
	void OnSetFocus(HWND hWndLoseFocus);
	void OnKillFocus(HWND hWndGetFocus);
	void OnKeyDown(UINT virtKey, WORD repCount, UINT flags);
	LRESULT OnGetDlgCode(UINT virtKey, MSG *pMsg);
	void OnLButtonDblClk(DWORD keys, short x, short y);
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);

	void PrepareDraw();
	void Draw();

	RMemoryDC m_mdc;
	int m_nItemHeight, m_nMargin, m_xOffset, m_yOffset, m_cxList, m_cyList;
	RSprite m_sprBackgr, m_sprSelected, m_sprSelectedNotFocus;
	RObArray<RString> m_items;
	RScrollBar m_sbHorz, m_sbVert;
	INT_PTR m_nSel, m_nEdit;
	bool m_bAlwaysShowSel;
	CLBEdit m_edit;
	RButton m_btn;
};
