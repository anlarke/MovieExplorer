#pragma once

class CStatusBar : public RWindow
{
	friend class RWindow;

public:
	CStatusBar();
	~CStatusBar();
	int GetHeight();

protected:
	bool OnCreate(CREATESTRUCT *pCS);
	void OnPaint(HDC hDC);
	void OnPrefChanged();
	void OnScaleChanged();
	LRESULT OnSetText(const TCHAR *lpszText);
	void OnSize(DWORD type, WORD cx, WORD cy);

	void Draw();

	RMemoryDC m_mdc;
	bool m_bBackgrWinDefault;
	COLORREF m_clrBackgrT, m_clrBackgrB, m_clrBackgrLine, m_clrText;
	RFont m_font;
	int m_nHeight;
};
