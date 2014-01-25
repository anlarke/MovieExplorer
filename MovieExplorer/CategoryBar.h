#pragma once

class CCategoryBar : public RWindow
{
	friend class RWindow;

public:
	CCategoryBar();
	~CCategoryBar();

protected:
	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);
	bool OnCreate(CREATESTRUCT *pCS);
	void OnDestroy();
	void OnMouseMove(DWORD keys, short x, short y);
	void OnLButtonDown(DWORD keys, short x, short y);
	void OnPaint(HDC hDC);
	void OnPrefChanged();
	void OnSize(DWORD type, WORD cx, WORD cy);

	void FilterDB();
	void PrepareDraw();
	void CalcRects();
	void Draw();
	void SelectButton(INT_PTR nIndex, bool bToggle);
	void Update(bool bResetSelections = false);

	struct BUTTON
	{
		RString strText, strText2;
		RRect rc;
		UINT_PTR state;
	};

	RMemoryDC m_mdc, m_mdcExpandIcon;
	RObArray<BUTTON> m_buttons;
	RSprite m_sprNormal, m_sprHighlight, m_sprSelected;
	RFont m_font;
	COLORREF m_clrNormalText, m_clrNormalText2, m_clrHighlightText, m_clrHighlightText2,
			m_clrSelectedText, m_clrSelectedText2;
	HMENU m_hMenu;
};
