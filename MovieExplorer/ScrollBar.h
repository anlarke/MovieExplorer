#pragma once

class CScrollBar : public RWindow
{
	friend class RWindow;
	friend class RControl;

public:
	CScrollBar();
	~CScrollBar();

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, bool bHorz = false)
	{
		m_bHorz = bHorz;
		if (!RWindow::Create<DERIVED_TYPE>(hWndParent))
			ASSERTRETURN(false);
		return true;
	}

	int SetScrollInfo(int nRangeMin, int nRangeMax, int nPageSize = -1, int nPos = -1);
	void SetPos(int nPos, bool bRedraw = true);
	int GetPos() const;
	int GetTrackPos() const;

protected:
	bool OnCreate(CREATESTRUCT *pCS);
	void OnLButtonDblClk(DWORD keys, short x, short y);
	void OnLButtonDown(DWORD keys, short x, short y);
	void OnLButtonUp(DWORD keys, short x, short y);
	void OnMouseMove(DWORD keys, short x, short y);
	void OnPaint(HDC hDC);
	void OnPrefChanged();
	void OnSize(DWORD type, WORD cx, WORD cy);
	void OnTimer(UINT_PTR nIDEvent);
	void OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar);

	void CalcRects(int y = 0);
	void Draw();
	void PrepareDraw();

	bool m_bHorz, m_bMouseOver, m_bWindowsDefault, m_bDragging, m_bTimer;
	int m_nRangeMin, m_nRangeMax, m_nPageSize, m_nPos, m_nTrackPos;
	RMemoryDC m_mdc, m_mdcGripper, m_mdcArrowUpActive, m_mdcArrowUpInactive, 
			m_mdcArrowDownActive, m_mdcArrowDownInactive;
	RSprite m_sprBackgr, m_sprThumbNormal, m_sprThumbHighlight, m_sprThumbPressed;
	RRect m_rcThumb, m_rcBtn1, m_rcBtn2, m_rcBtnPageUp, m_rcBtnPageDown, m_rcDragging;
	INT_PTR m_nHighlight, m_nPressed;
	RScrollBar m_sb;
	POINT m_ptDragging;
};
