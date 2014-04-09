#include "stdafx.h"
#include "MovieExplorer.h"
#include "StatusBar.h"

CStatusBar::CStatusBar() : m_clrBackgrT(0), m_clrBackgrB(0), m_clrBackgrLine(0), m_clrText(0), 
						   m_nHeight(0)
{
}

CStatusBar::~CStatusBar()
{
}

int CStatusBar::GetHeight()
{
	return m_nHeight;
}

bool CStatusBar::OnCreate(CREATESTRUCT *pCS)
{
	UNREFERENCED_PARAMETER(pCS);
	m_mdc.Create(0, 0);
	OnPrefChanged();
	return true;
}

void CStatusBar::OnPaint(HDC hDC)
{
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	BitBlt(hDC, &rc, m_mdc);
}

void CStatusBar::OnPrefChanged()
{
	m_bBackgrWinDefault = GETTHEMEBOOL(_T("StatusBar"), _T("BackgrWindowsDefault"));
	m_clrBackgrT = GETTHEMECOLOR(_T("StatusBar"), _T("BackgrTopColor"));
	m_clrBackgrB = GETTHEMECOLOR(_T("StatusBar"), _T("BackgrBottomColor"));
	m_clrBackgrLine = GETTHEMECOLOR(_T("StatusBar"), _T("BackgrLineColor"));
	m_clrText = GETTHEMECOLOR(_T("StatusBar"), _T("TextColor"));
	SetText(GETSTR(IDS_READY));

	OnScaleChanged();
}

void CStatusBar::OnScaleChanged()
{
	GETTHEMEFONT(&m_font, _T("StatusBar"), _T("Font"));

	HFONT hPrevFont = (HFONT)SelectObject(m_mdc, m_font);
	SIZE sz;
	GetTextExtentPoint32(m_mdc, _T("Ag"), 2, &sz);
	m_nHeight = SCY(4) + sz.cy + SCY(4);
	SelectObject(m_mdc, hPrevFont);

	Draw();
}

LRESULT CStatusBar::OnSetText(const TCHAR *lpszText)
{
	LRESULT res = RWindow::OnSetText(lpszText);
	Draw();
	return res;
}

void CStatusBar::OnSize(DWORD type, WORD cx, WORD cy)
{
	UNREFERENCED_PARAMETER(type);

	m_mdc.Create(cx, cy);

	if (cx == 0 || cy == 0)
		return;

	Draw();
}

void CStatusBar::Draw()
{
	const int& cx = m_mdc.cx;
	const int& cy = m_mdc.cy;

	// Draw background

	if (m_bBackgrWinDefault)
	{
		DrawRect(m_mdc, 0, 0, cx, 1, GetSysColor(COLOR_3DSHADOW));
		DrawRect(m_mdc, 0, 1, cx, 1, GetSysColor(COLOR_3DHIGHLIGHT));
		DrawRect(m_mdc, 0, 2, cx, cy - 2, GetSysColor(COLOR_3DFACE));
	}
	else
	{
		DrawRectGradientV(m_mdc, 0, 0, cx, cy, m_clrBackgrT, m_clrBackgrB);
		DrawRect(m_mdc, 0, 0, cx, SCY(1), m_clrBackgrLine);
	}

	// Draw status text

	int nSavedDC = SaveDC(m_mdc);

	SetTextColor(m_mdc, m_clrText);
	SelectObject(m_mdc, m_font);
	TextOut(m_mdc, SCX(4), SCY(4), GetText());

	RestoreDC(m_mdc, nSavedDC);

	Invalidate(m_hWnd);
	PostMessage(m_hWnd, WM_PAINT);
}
