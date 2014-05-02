#include "stdafx.h"
#include "MovieExplorer.h"
#include "ToolBarTreeView.h"

CToolBarTreeView::CToolBarTreeView()
{
}

CToolBarTreeView::~CToolBarTreeView()
{
}

void CToolBarTreeView::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	PostMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(id, notifyCode), (LPARAM)hWndControl);
}

bool CToolBarTreeView::Create(HWND hWndParent, DWORD style, DWORD exStyle)
{
	if (!RWindow::Create<CToolBarTreeView>(hWndParent))
		ASSERTRETURN(false);

	if (!m_tb.Create<RCustomToolBar>(m_hWnd))
		ASSERTRETURN(false);

	style &= ~WS_BORDER;
	exStyle &= ~WS_EX_CLIENTEDGE;

	if (!m_tv.Create<RTreeView>(m_hWnd, style, exStyle))
		ASSERTRETURN(false);

	return true;
}

void CToolBarTreeView::OnSize(DWORD type, WORD cx, WORD cy)
{
	UNREFERENCED_PARAMETER(type);

	m_mdc.Create(cx, cy);
	Draw();

	// TODO: detect size of border
	const int x = 2;
	const int y = 2;
	const int cx2 = cx - 2 * x;
	const int cy2 = cy - 2 * y;

	MoveWindow(m_tb, x, y, cx2, SCY(24));
	MoveWindow(m_tv, x, y + SCY(24), cx2, cy2 - SCY(24));

	PostMessage(m_hWnd, WM_PAINT);
	PostChildren(m_hWnd, WM_PAINT);
}

void CToolBarTreeView::OnPaint(HDC hDC)
{
	m_mdc.Draw(hDC);
}

LRESULT CToolBarTreeView::OnNotify(UINT_PTR id, NMHDR *pNMH)
{
	return SendMessage(GetParent(m_hWnd), WM_NOTIFY, id, (LRESULT)pNMH);
}

void CToolBarTreeView::Draw()
{
	const int& cx = m_mdc.cx;
	const int& cy = m_mdc.cy;

	if (IsAppThemed())
	{
		HTHEME hTheme = OpenThemeData(m_hWnd, L"TREEVIEW");
		ASSERT(hTheme);
		RECT rc = {0, 0, cx, cy};
		DrawThemeBackground(hTheme, m_mdc, LBCP_BORDER_NOSCROLL, LBPSN_NORMAL, &rc, NULL);
		CloseThemeData(hTheme);

		// TODO: detect size of border (check for high DPI)
		DrawRect(m_mdc, 1, 1, cx - 2, SCY(24), GetSysColor(COLOR_BTNFACE), 96);
		DrawRect(m_mdc, 1, 1 + SCY(24) - 1, cx - 2, 1, GetSysColor(COLOR_BTNFACE), 192);
	}
	else
	{
		RECT rc = {0, 0, cx, cy};
		FillSolidRect(m_mdc, &rc, GetSysColor(COLOR_WINDOW));
		FillSolidRect(m_mdc, 1, 1, cx - 2, SCY(24), GetSysColor(COLOR_BTNFACE));
		DrawEdge(m_mdc, &rc, EDGE_SUNKEN, BF_RECT);
	}
}
