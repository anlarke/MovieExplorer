#include "stdafx.h"
#include "MovieExplorer.h"
#include "EditListBox.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// CLBEdit

void CLBEdit::OnKillFocus(HWND hWndGetFocus)
{
	REdit::OnKillFocus(hWndGetFocus);
	if (GetParent(hWndGetFocus) != GetParent(m_hWnd))
		PostMessage(GetParent(m_hWnd), WM_APPLYEDIT);
}

LRESULT CLBEdit::OnGetDlgCode(UINT virtKey, MSG *pMsg)
{
	UNREFERENCED_PARAMETER(virtKey);
	UNREFERENCED_PARAMETER(pMsg);
	return DLGC_WANTALLKEYS;
}

void CLBEdit::OnKeyDown(UINT virtKey, WORD repCount, UINT flags)
{
	if (virtKey == VK_ESCAPE)
	{
		PostMessage(GetParent(m_hWnd), WM_CANCELEDIT);
	}
	else if (virtKey == VK_RETURN)
	{
		PostMessage(GetParent(m_hWnd), WM_APPLYEDIT);
	}
	else
		REdit::OnKeyDown(virtKey, repCount, flags);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CEditListBox

CEditListBox::CEditListBox() : m_nItemHeight(24), m_nMargin(4), m_xOffset(0), m_yOffset(0), 
		m_cxList(0), m_cyList(0), m_nSel(-1), m_nEdit(-1), m_bAlwaysShowSel(true)
{
}

bool CEditListBox::Create(HWND hWndParent, bool bShowSelAlways /*= false*/, 
		const TCHAR *lpszEditBtnText /*= NULL*/)
{
	if (!RWindow::Create<CEditListBox>(hWndParent, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|
			WS_CLIPSIBLINGS|WS_TABSTOP))
		ASSERTRETURN(false);

	m_bAlwaysShowSel = bShowSelAlways;
	m_btn.SetText(lpszEditBtnText);

	return true;
}

INT_PTR CEditListBox::Add(const TCHAR *lpszText)
{
	INT_PTR nIndex = m_items.Add(lpszText);
	
	m_cyList += m_nItemHeight;
	
	SIZE sz;
	ASSERT(m_mdc);
	HFONT hPrevFont = (HFONT)SelectObject(m_mdc, GetDialogFont());
	GetTextExtentPoint32(m_mdc, m_items[nIndex], &sz);
	SelectObject(m_mdc, hPrevFont);
	int cxList = 2 * S(m_nMargin) + sz.cx;
	m_cxList = max(cxList, m_cxList);

	if (m_nSel == -1)
		m_nSel = nIndex;

	Draw();

	return nIndex;
}

bool CEditListBox::SetSel(INT_PTR nIndex)
{
	if (nIndex < 0 || nIndex >= m_items)
		return false;

	if (m_nEdit != -1)
		return false;
	
	m_nSel = nIndex;

	// Make sure item is visible

	int cyThumb = GetSystemMetrics(SM_CYVTHUMB);

	int cyAvailable = m_mdc.cy - 4 - (IsWindowVisible(m_sbHorz) ? cyThumb : 0);
	int yItem = 2 + (int)nIndex * S(m_nItemHeight) - m_yOffset;

	if (yItem < 2)
	{
		m_yOffset = max((int)nIndex * S(m_nItemHeight), 0);
		m_sbVert.SetPos(m_yOffset);
	}
	else if (yItem + S(m_nItemHeight) > 2 + cyAvailable)
	{
		m_yOffset = max((int)(nIndex + 1) * S(m_nItemHeight) - cyAvailable, 0);
		m_sbVert.SetPos(m_yOffset);
	}

	// Redraw and notify parent

	Draw();
	PostMessage(m_hWnd, WM_COMMAND, MAKEWPARAM((WORD)GetMenu(m_hWnd), LBN_SELCHANGE), 
			(LPARAM)m_hWnd);

	return true;
}

INT_PTR CEditListBox::GetSel() const
{
	return m_nSel;
}

bool CEditListBox::Edit(INT_PTR nIndex)
{
	if (nIndex < 0 || nIndex >= m_items)
		return false;

	if (m_nEdit != -1)
		return false;

	SetSel(nIndex);
	m_nEdit = nIndex;

	int cxThumb = GetSystemMetrics(SM_CXHTHUMB);
	int cxClippedList = m_mdc.cx - 2 - (IsWindowVisible(m_sbVert) ? cxThumb : 0);

	int xEdit = 1;
	int yEdit = 2 + (int)nIndex * S(m_nItemHeight) - m_yOffset + S(2);
	int cxEdit = cxClippedList - S(1);
	int cyEdit = S(m_nItemHeight) - S(5);

	m_edit.SetText(m_items[nIndex]);

	if (m_btn.GetText().IsEmpty())
		MoveWindow(m_edit, xEdit, yEdit, cxEdit, cyEdit);
	else
	{
		MoveWindow(m_edit, xEdit, yEdit, cxEdit - S(27), cyEdit);
		MoveWindow(m_btn, cxClippedList - S(26), yEdit-1, S(24), cyEdit+2);
	}

	SetFocus(m_edit);
	m_edit.SetSel(0);
	
	return true;
}

CLBEdit* CEditListBox::GetEditControl()
{
	return &m_edit;
}

LRESULT CEditListBox::WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_APPLYEDIT)
	{
		if (m_nEdit >= 0 && m_nEdit < m_items)
		{
			if (m_items[m_nEdit] != m_edit.GetText())
			{
				m_items[m_nEdit] = m_edit.GetText();
				PostMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(
						(WORD)GetMenu(m_hWnd), LBN_TEXTCHANGE), (LPARAM)m_hWnd);
			}
		}

		if (GetFocus() == m_edit)
			SetFocus(m_hWnd);
		
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		OnSize(0, (WORD)rc.right, (WORD)rc.bottom);

		return 0;
	}
	else if (Msg == WM_CANCELEDIT)
	{
		if (GetFocus() == m_edit)
			SetFocus(m_hWnd);

		MoveWindow(m_edit, 0, 0, 0, 0);
		MoveWindow(m_btn, 0, 0, 0, 0);
		m_nEdit = -1;
		Draw();
		return 0;
	}
	return RWindow::WndProc(Msg, wParam, lParam);
}

bool CEditListBox::OnCreate(CREATESTRUCT *pCS)
{
	UNREFERENCED_PARAMETER(pCS);

	if (!m_sbHorz.Create<RScrollBar>(m_hWnd, true) ||
			!m_sbVert.Create<RScrollBar>(m_hWnd, false) ||
			!m_edit.Create<CLBEdit>(m_hWnd, WS_BORDER|ES_AUTOHSCROLL) ||
			!m_btn.Create<RButton>(m_hWnd))
		ASSERTRETURN(false);

	if (IsAppThemed())
		SetWindowTheme(m_hWnd, L"Explorer", NULL);

	PrepareDraw();

	return true;
}

void CEditListBox::OnSize(DWORD type, WORD cx, WORD cy)
{
	UNREFERENCED_PARAMETER(type);

	m_mdc.Create(cx, cy);

	// Calculate width of largest item

	SIZE sz;
	int cxList;
	HFONT hPrevFont = (HFONT)SelectObject(m_mdc, GetDialogFont());

	m_cxList = 0;
	foreach (m_items, strText)
	{
		GetTextExtentPoint32(m_mdc, strText, &sz);
		cxList = 2 * S(m_nMargin) + sz.cx;
		m_cxList = max(cxList, m_cxList);
	}

	SelectObject(m_mdc, hPrevFont);

	// Decide whether the scrollbars should be shown

	int cxThumb = GetSystemMetrics(SM_CXHTHUMB);
	int cyThumb = GetSystemMetrics(SM_CYVTHUMB);

	bool bShowHorz = (m_cxList > cx - 4);
	bool bShowVert = (m_cyList > cy - 4);
	bShowHorz = (m_cxList > cx - 4 - (bShowVert ? cxThumb : 0));
	bShowVert = (m_cyList > cy - 4 - (bShowHorz ? cyThumb : 0));

	MoveWindow(m_sbHorz, 2, cy - 2 - cyThumb, cx - 4 - (bShowVert ? cxThumb : 0), cyThumb);
	ShowWindow(m_sbHorz, (bShowHorz ? SW_SHOW : SW_HIDE));

	MoveWindow(m_sbVert, cx - 2 - cxThumb, 2, cxThumb, cy - 4 - (bShowHorz ? cyThumb : 0));
	ShowWindow(m_sbVert, (bShowVert ? SW_SHOW : SW_HIDE));

	// Set scrollbar info

	m_sbHorz.SetScrollInfo(0, m_cxList - 1, cx - 4 - (bShowVert ? cxThumb : 0));
	m_sbVert.SetScrollInfo(0, m_cyList - 1, cy - 4 - (bShowHorz ? cyThumb : 0));

	m_xOffset = m_sbHorz.GetPos();
	m_yOffset = m_sbVert.GetPos();

	// Cancel edit operation

	m_nEdit = -1;
	MoveWindow(m_edit, 0, 0, 0, 0);
	MoveWindow(m_btn, 0, 0, 0, 0);
	
	Draw();
	PostChildren(m_hWnd, WM_PAINT);
}

void CEditListBox::OnPaint(HDC hDC)
{
	m_mdc.Draw(hDC);
}

void CEditListBox::OnHScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
{
	UNREFERENCED_PARAMETER(scrollCode);
	UNREFERENCED_PARAMETER(pos);
	UNREFERENCED_PARAMETER(hWndScrollBar);

	m_sbHorz.SetPos(m_sbHorz.GetTrackPos());
	m_xOffset = m_sbHorz.GetPos();
	Draw();
}

void CEditListBox::OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
{
	UNREFERENCED_PARAMETER(scrollCode);
	UNREFERENCED_PARAMETER(pos);
	UNREFERENCED_PARAMETER(hWndScrollBar);

	m_sbVert.SetPos(m_sbVert.GetTrackPos());
	m_yOffset = m_sbVert.GetPos();
	Draw();
}

void CEditListBox::OnLButtonDown(DWORD keys, short x, short y)
{
	UNREFERENCED_PARAMETER(keys);
	UNREFERENCED_PARAMETER(x);

	SetFocus(m_hWnd);
	int yItem = m_yOffset + y - 1;
	INT_PTR nItem = yItem / S(m_nItemHeight);
	SetSel(nItem);
}

void CEditListBox::OnLButtonDblClk(DWORD keys, short x, short y)
{
	UNREFERENCED_PARAMETER(keys);
	UNREFERENCED_PARAMETER(x);

	int yItem = m_yOffset + y - 1;
	INT_PTR nItem = yItem / S(m_nItemHeight);
	Edit(nItem);
}

void CEditListBox::OnSetFocus(HWND hWndLoseFocus)
{
	UNREFERENCED_PARAMETER(hWndLoseFocus);
	Draw();
}

void CEditListBox::OnKillFocus(HWND hWndGetFocus)
{
	UNREFERENCED_PARAMETER(hWndGetFocus);
	Draw();
}

void CEditListBox::OnKeyDown(UINT virtKey, WORD repCount, UINT flags)
{
	UNREFERENCED_PARAMETER(repCount);
	UNREFERENCED_PARAMETER(flags);
	if (virtKey == VK_UP)
		SetSel(m_nSel - 1);
	else if (virtKey == VK_DOWN)
		SetSel(m_nSel + 1);
	else if (virtKey == VK_F2)
		Edit(m_nSel);
}

LRESULT CEditListBox::OnGetDlgCode(UINT virtKey, MSG *pMsg)
{
	UNREFERENCED_PARAMETER(virtKey);
	UNREFERENCED_PARAMETER(pMsg);
	return DLGC_WANTARROWS;
}

void CEditListBox::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	UNREFERENCED_PARAMETER(id);
	UNREFERENCED_PARAMETER(notifyCode);

	if (hWndControl == m_btn)
		PostMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(
				(WORD)GetMenu(m_hWnd), LBN_BTNCLICKED), (LPARAM)m_hWnd);
}

void CEditListBox::PrepareDraw()
{
	m_sprBackgr.Create(5, 5, 2, 2, 2, 2);
	m_sprSelected.Create(S(m_nItemHeight), S(m_nItemHeight), S(4), S(4), S(4), S(4));
	m_sprSelectedNotFocus.Create(S(m_nItemHeight), S(m_nItemHeight), S(4), S(4), S(4), S(4));
	
	RECT rcBackgr = {0, 0, 5, 5};
	RECT rc = {0, 0, S(m_nItemHeight), S(m_nItemHeight)};

	if (IsAppThemed() && !IsWinXP())
	{
		HTHEME hTheme = OpenThemeData(m_hWnd, L"TREEVIEW");

		DrawThemeBackground(hTheme, m_sprBackgr, LBCP_BORDER_NOSCROLL, LBPSN_NORMAL, &rcBackgr, NULL);

		DrawRect(m_sprSelected, &rc, GetSysColor(COLOR_WINDOW));
		DrawThemeBackground(hTheme, m_sprSelected, TVP_TREEITEM,  TREIS_HOTSELECTED, &rc, NULL);

		DrawRect(m_sprSelectedNotFocus, &rc, GetSysColor(COLOR_WINDOW));
		DrawThemeBackground(hTheme, m_sprSelectedNotFocus, TVP_TREEITEM, 
				TREIS_SELECTEDNOTFOCUS, &rc, NULL);

		CloseThemeData(hTheme);
	}
	else
	{
		DrawRect(m_sprBackgr, &rcBackgr, GetSysColor(COLOR_WINDOW));
		DrawEdge(m_sprBackgr, &rcBackgr, EDGE_SUNKEN, BF_RECT);
		DrawRect(m_sprSelected, &rc, GetSysColor(COLOR_HIGHLIGHT));
		//DrawRect(m_sprSelectedNotFocus, &rc, GetSysColor(COLOR_WINDOW));
		DrawRect(m_sprSelectedNotFocus, &rc, GetSysColor(COLOR_BTNFACE));//, 128);
	}

	InflateRect(&rcBackgr, -2, -2);
	DrawRect(m_sprBackgr, &rcBackgr, GetSysColor(COLOR_WINDOW));
}

void CEditListBox::Draw()
{
	// Draw background

	RECT rc;
	GetClientRect(m_hWnd, &rc);

	m_sprBackgr.Draw(m_mdc, &rc);

	// Set DC origin and clipping rect

	int cxThumb = GetSystemMetrics(SM_CXHTHUMB);
	int cyThumb = GetSystemMetrics(SM_CYVTHUMB);

	int cx = rc.right - 4 - (IsWindowVisible(m_sbVert) ? cxThumb : 0);
	int cy = rc.bottom - 4 - (IsWindowVisible(m_sbHorz) ? cyThumb : 0);

	SetWindowOrgEx(m_mdc, -2, -2, NULL);
	HRGN hRgn = CreateRectRgn(2, 2, 2 + cx, 2 + cy);
	SelectClipRgn(m_mdc, hRgn);

	// Draw items

	INT_PTR nStart = max(m_yOffset / S(m_nItemHeight), 0);
	int y = (int)nStart * S(m_nItemHeight) - m_yOffset;
	HWND hWndFocus = GetFocus();
	bool bFocus = (hWndFocus == m_hWnd || hWndFocus == m_edit || hWndFocus == m_btn);
	HFONT hPrevFont = (HFONT)SelectObject(m_mdc, GetDialogFont());
	SIZE sz;
	GetTextExtentPoint32(m_mdc, _T("Ag"), &sz);

	for (INT_PTR i = nStart; i < m_items && y < cy; ++i, y += S(m_nItemHeight))
	{
		SetTextColor(m_mdc, GetSysColor(COLOR_WINDOWTEXT));

		if (i == m_nSel)
		{
			if (bFocus)
			{
				m_sprSelected.Draw(m_mdc, -m_xOffset, y, max(m_cxList, cx), S(m_nItemHeight));
				if (!IsAppThemed() || IsWinXP())
					SetTextColor(m_mdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
			else if (m_bAlwaysShowSel)
				m_sprSelectedNotFocus.Draw(m_mdc, -m_xOffset, y, max(m_cxList, cx), S(m_nItemHeight));
		}

		if (i != m_nEdit)
			TextOut(m_mdc, -m_xOffset + S(m_nMargin), y + (S(m_nItemHeight) - sz.cy) / 2, m_items[i]);
	}

	SelectObject(m_mdc, hPrevFont);

	// Restore DC origin and remove clipping rect

	SetWindowOrgEx(m_mdc, 0, 0, NULL);
	SelectClipRgn(m_mdc, NULL);
	DeleteObject(hRgn);

	// Update window
	
	Invalidate(m_hWnd);
	PostMessage(m_hWnd, WM_PAINT);
}
