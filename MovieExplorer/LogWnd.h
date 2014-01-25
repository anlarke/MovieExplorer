#pragma once

class CLogWnd : public RWindow
{
	friend class RWindow;

public:
	CLogWnd();
	~CLogWnd();
	bool Write(const TCHAR* lpszText, UINT_PTR type = 0);

protected:
	HBRUSH OnCtlColorStatic(HDC hDC, HWND hWnd);
	bool OnCreate(CREATESTRUCT *pCS);
	void OnDestroy();
	void OnPaint(HDC hDC);
	void OnPrefChanged();
	void OnScaleChanged();
	void OnSize(DWORD type, WORD cx, WORD cy);

	COLORREF m_clrBackgrLine, m_clrText;
	RFont m_font;
	REditFF m_edit;
	HBRUSH m_hbrBackgr;
	RString m_strText;
};
