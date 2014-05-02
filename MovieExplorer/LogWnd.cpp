#include "stdafx.h"
#include "MovieExplorer.h"
#include "LogWnd.h"

CLogWnd::CLogWnd() : m_hbrBackgr(NULL)
{
}

CLogWnd::~CLogWnd()
{
}

bool CLogWnd::Write(const TCHAR* lpszText, UINT_PTR type /*= 0*/)
{
	UNREFERENCED_PARAMETER(type);

	if (!IsWindow(m_hWnd))
		ASSERTRETURN(false);

	m_strText += FixLineEnds(lpszText);
	if (!m_edit.SetText(m_strText))
		ASSERTRETURN(false);

	PostMessage(m_edit, WM_VSCROLL, SB_BOTTOM);

	return true;
}

HBRUSH CLogWnd::OnCtlColorStatic(HDC hDC, HWND hWnd)
{
	UNREFERENCED_PARAMETER(hWnd);
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, m_clrText);
	return m_hbrBackgr;
}

bool CLogWnd::OnCreate(CREATESTRUCT *pCS)
{
	UNREFERENCED_PARAMETER(pCS);

	if (!m_edit.Create<REditFF>(m_hWnd, ES_MULTILINE|ES_READONLY|ES_AUTOHSCROLL|
			ES_AUTOVSCROLL/*|WS_VSCROLL*/))
		ASSERTRETURN(false);

	OnPrefChanged();
	return true;
}

void CLogWnd::OnDestroy()
{
	DeleteObject(m_hbrBackgr);
}

void CLogWnd::OnPaint(HDC hDC)
{
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	FillSolidRect(hDC, 0, 0, rc.right, SCY(1), m_clrBackgrLine);
}

void CLogWnd::OnPrefChanged()
{
	DeleteObject(m_hbrBackgr);
	m_hbrBackgr = CreateSolidBrush(GETTHEMECOLOR(_T("LogWnd"), _T("BackgrColor")));
	m_clrBackgrLine = GETTHEMECOLOR(_T("LogWnd"), _T("BackgrLineColor"));
	m_clrText = GETTHEMECOLOR(_T("LogWnd"), _T("TextColor"));
	
	DWORD dwStyle = (DWORD)GetWindowLong(m_edit, GWL_STYLE);
	if (GETTHEMEBOOL(_T("LogWnd"), _T("ShowScrollBar")))
		dwStyle |= WS_VSCROLL;
	else
		dwStyle &= ~WS_VSCROLL;
	SetWindowLong(m_edit, GWL_STYLE, (LONG)dwStyle);

	OnScaleChanged();
}

void CLogWnd::OnScaleChanged()
{
	GETTHEMEFONT(&m_font, _T("LogWnd"), _T("Font"));
	m_edit.SetFont(m_font);

	MoveWindow(m_edit, 0, 0, 0, 0);
	Invalidate(m_hWnd);
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
}

void CLogWnd::OnSize(DWORD type, WORD cx, WORD cy)
{
	UNREFERENCED_PARAMETER(type);

	MoveWindow(m_edit, 0, SCY(1), cx, cy - SCY(1));

	PostMessage(m_hWnd, WM_PAINT);
	PostChildren(m_hWnd, WM_PAINT);
}
