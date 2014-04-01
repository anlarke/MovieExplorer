#pragma once

#include "ScrollBar.h"

class CGridView : public RWindow
{
	friend class RWindow;

public:

	CGridView();
	~CGridView();

protected:

	bool OnCreate(CREATESTRUCT *pCS);
	void OnPaint(HDC hDC);
	void OnPrefChanged();
	void OnScaleChanged();
	void OnMouseWheel(WORD keys, short delta, short x, short y);
	void OnMouseMove(DWORD keys, short x, short y);
	void OnLButtonDown(DWORD keys, short x, short y);
	void OnKeyDown(UINT virtKey, WORD repCount, UINT flags);
	bool OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg);
	void OnSize(DWORD type, WORD cx, WORD cy);
	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	void OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar);
	void Draw();

	RMemoryDC m_mdc;
	INT_PTR m_nHoverMov;
	bool m_bScrolling, m_bCaptureM;

	COLORREF m_clrBackgr, m_clrBackgrAlt, m_clrShadow;
	BYTE m_aPosterShadow;
	RSprite m_sprShadow;
	struct LINK { RString strText, strURL; RRect rc; UINT_PTR state; };
	RObArray<LINK> m_links;
	CScrollBar m_sb;
	RObArray<RString> m_servicesInUse;

};