#include "stdafx.h"
#include "MovieExplorer.h"
#include "CategoryBar.h"

// TODO: selectie op menu knop blijft hangen als je menu escaped door ergens anders te klikken

#define CB_TEXTMARGIN		SCX(10)
#define CB_MENUBUTTONWIDTH	SCX(18)

#define CB_MOVIE_BUTTON		1
#define CB_TV_BUTTON		2

CCategoryBar::CCategoryBar() : m_clrNormalText(0), m_clrNormalText2(0), m_clrHighlightText(0),
	m_clrHighlightText2(0), m_clrSelectedText(0), m_clrSelectedText2(0), m_hMenu(NULL), m_nAutoCategories(3)
{
}

CCategoryBar::~CCategoryBar()
{
}

LRESULT CCategoryBar::WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_DBUPDATED)
	{
		Update((bool)wParam);
		return 0;
	}
	return RWindow::WndProc(Msg, wParam, lParam);
}

void CCategoryBar::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	UNREFERENCED_PARAMETER(notifyCode);
	UNREFERENCED_PARAMETER(hWndControl);

	if (id >= m_buttons-1)
		{ASSERT(false); return;}

	// Toggle checked state of menu items that represent the buttons
		
	MENUITEMINFO mii;
	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	if (GetMenuItemInfo(m_hMenu, id, FALSE, &mii))
	{
		if (mii.fState & MFS_CHECKED)
		{
			SelectButton(id, true);
			mii.fState = MFS_UNCHECKED;
		}
		else
		{
			SelectButton(id, true);
			mii.fState = MFS_CHECKED;
		}
		SetMenuItemInfo(m_hMenu, id, FALSE, &mii);
		FilterDB();
	}
}

bool CCategoryBar::OnCreate(CREATESTRUCT *pCS)
{
	UNREFERENCED_PARAMETER(pCS);
	m_nAutoCategories = GETPREFBOOL(_T("AutoCategories"))?3:1;
	m_mdc.Create(0, 0);
	Update();
	return true;
}

void CCategoryBar::OnDestroy()
{
	DestroyMenu(m_hMenu);
	m_mdc.Destroy(); // releases selected GDI objects
}

void CCategoryBar::OnMouseMove(DWORD keys, short x, short y)
{
	UNREFERENCED_PARAMETER(keys);

	bool bOnButton = false;

	foreach (m_buttons, button)
	{
		if (PtInRect(&button.rc, x, y))
		{
			bOnButton = true;

			if (button.state == CTBBSTATE_NORMAL)
			{
				button.state = CTBBSTATE_HIGHLIGHT;
				Draw();
				Invalidate(m_hWnd);
			}
		}
		else
		{
			if (button.state == CTBBSTATE_HIGHLIGHT)
			{
				button.state = CTBBSTATE_NORMAL;
				Draw();
				Invalidate(m_hWnd);
			}
		}
	}

	if (bOnButton)
		SetCapture(m_hWnd);
	else
		ReleaseCapture();
}

void CCategoryBar::OnLButtonDown(DWORD keys, short x, short y)
{
	UNREFERENCED_PARAMETER(keys);

	SetFocus(m_hWnd);

	foreach (m_buttons, button, i)
	{
		if (!PtInRect(&button.rc, x, y))
			continue;
		
		if (i == m_buttons-1) // menu button is clicked
		{
			if (button.state == CTBBSTATE_HIGHLIGHT)
			{
				button.state = CTBBSTATE_SELECTED;
				Draw();
				Invalidate(m_hWnd);
				RECT rc = button.rc;
				ClientToScreen(m_hWnd, &rc);
				TrackPopupMenu(m_hMenu, TPM_LEFTALIGN|TPM_TOPALIGN, rc.right, rc.top, 0, m_hWnd, NULL);
				button.state = CTBBSTATE_NORMAL;
			}
		}
		else
			SelectButton(i, GetKeyState(VK_CONTROL) < 0);
	}

	SetFocus(GetMainWnd());
}

void CCategoryBar::OnPaint(HDC hDC)
{
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	BitBlt(hDC, &rc, m_mdc);
}

void CCategoryBar::OnPrefChanged()
{
	PrepareDraw();
	Draw();
	Invalidate(m_hWnd);
}

void CCategoryBar::OnSize(DWORD type, WORD cx, WORD cy)
{
	UNREFERENCED_PARAMETER(type);

	bool bHeightChanged = (cy != m_mdc.cx);

	VERIFY(m_mdc.Create(cx, cy));

	if (cx == 0 || cy == 0)
		return;

	if (bHeightChanged)
		PrepareDraw();
	Draw();
	Invalidate(m_hWnd);
}

void CCategoryBar::FilterDB()
{
	if (m_buttons == 0)
		{ASSERT(false); return;}

	RArray<INT_PTR> categories;
	if (m_buttons[0].state != CTBBSTATE_SELECTED)
	{
		for (INT_PTR i = m_nAutoCategories; i < m_buttons-1; ++i)
			if (m_buttons[i].state == CTBBSTATE_SELECTED)
				categories.Add(i - m_nAutoCategories);
	}

	GetDB()->SetOnlyTV(false);
	GetDB()->SetOnlyMovies(false);
	if (m_nAutoCategories > CB_MOVIE_BUTTON && m_buttons[CB_MOVIE_BUTTON].state == CTBBSTATE_SELECTED)
		GetDB()->SetOnlyMovies(true);
	else if (m_nAutoCategories > CB_TV_BUTTON && m_buttons[CB_TV_BUTTON].state == CTBBSTATE_SELECTED)
		GetDB()->SetOnlyTV(true);
	GetDB()->FilterByCategories(categories);
}

void CCategoryBar::PrepareDraw()
{
	const int &cy = m_mdc.cy;

	if (cy < SCX(10))
		return;

	// Create normal sprite

	m_sprNormal.Create(cy, cy, 0, 0, SCX(1), 0);
	DrawRect(m_sprNormal, cy - SCX(1), 0, SCX(1), cy, GETTHEMECOLOR(_T("CategoryBarButton"), 
			_T("NormalRightEdgeColor")), roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), 
			_T("NormalRightEdgeAlpha")) * 255.0f));

	// Create highlight sprite

	m_sprHighlight.Create(cy, cy, SCX(3), 0, SCX(3), 0);

	DrawRectGradientV(m_sprHighlight, 0, 0, cy, cy, 
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightTopColor")),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightBottomColor")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightTopAlpha")) * 255.0f),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightBottomAlpha")) * 255.0f));

	DrawRectGradientV(m_sprHighlight, 0, 0, cy, cy / 2, 
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightHalfTopColor")),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightHalfBottomColor")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightHalfTopAlpha")) * 255.0f),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightHalfBottomAlpha")) * 255.0f));
		
	DrawRectGradientV(m_sprHighlight, 0, 0, cy, cy, 
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightBorder1TopColor")),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightBorder1BottomColor")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightBorder1TopAlpha")) * 255.0f),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightBorder1BottomAlpha")) * 255.0f), false);

	DrawRect(m_sprHighlight, SCX(1), SCX(1), cy - 2 * SCX(1), cy - 2 * SCX(1),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightBorder2Color")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightBorder2Alpha")) * 255.0f), false);

	DrawRect(m_sprHighlight, 2 * SCX(1), 2 * SCX(1), cy - 4 * SCX(1), cy - 4 * SCX(1),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightBorder3Color")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("HighlightBorder3Alpha")) * 255.0f), false);

	// Create selected sprite

	m_sprSelected.Create(cy, cy, SCX(3), 0, SCX(3), 0);

	DrawRectGradientV(m_sprSelected, 0, 0, cy, cy, 
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedTopColor")),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedBottomColor")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedTopAlpha")) * 255.0f),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedBottomAlpha")) * 255.0f));

	DrawRectGradientV(m_sprSelected, 0, 0, cy, cy / 2, 
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedHalfTopColor")),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedHalfBottomColor")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedHalfTopAlpha")) * 255.0f),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedHalfBottomAlpha")) * 255.0f));
		
	DrawRectGradientV(m_sprSelected, 0, 0, cy, cy, 
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedBorder1TopColor")),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedBorder1BottomColor")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedBorder1TopAlpha")) * 255.0f),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedBorder1BottomAlpha")) * 255.0f), false);

	DrawRect(m_sprSelected, SCX(1), SCX(1), cy - 2 * SCX(1), cy - 2 * SCX(1),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedBorder2Color")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedBorder2Alpha")) * 255.0f), false);

	DrawRect(m_sprSelected, 2 * SCX(1), 2 * SCX(1), cy - 4 * SCX(1), cy - 4 * SCX(1),
			GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedBorder3Color")),
			roundf(GETTHEMEFLOAT(_T("CategoryBarButton"), _T("SelectedBorder3Alpha")) * 255.0f), false);

	if (IsWin8() && GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedTopColor")) == 0xCA7919)
		DrawRect(m_sprSelected, 0, 0, S(1), cy, 0x000000, 20);

	// Get text font and color

	m_clrNormalText = GETTHEMECOLOR(_T("CategoryBarButton"), _T("NormalTextColor"));
	m_clrNormalText2 = GETTHEMECOLOR(_T("CategoryBarButton"), _T("NormalText2Color"));
	m_clrHighlightText = GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightTextColor"));
	m_clrHighlightText2 = GETTHEMECOLOR(_T("CategoryBarButton"), _T("HighlightText2Color"));
	m_clrSelectedText = GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedTextColor"));
	m_clrSelectedText2 = GETTHEMECOLOR(_T("CategoryBarButton"), _T("SelectedText2Color"));
	GETTHEMEFONT(&m_font, _T("CategoryBarButton"), _T("Font"));
	SelectObject(m_mdc, m_font);

	// Resize menu button icon

	RMemoryDC mdc(16, 16);
	DrawAlphaMap(mdc, 0, 0, IDA_EXPAND, 16, 16, m_clrNormalText);
	m_mdcExpandIcon.Create(SCX(8), SCY(8));
	Resize(&m_mdcExpandIcon, &mdc, true);
}

void CCategoryBar::Draw()
{
	const int &cx = m_mdc.cx;
	const int &cy = m_mdc.cy;
	SIZE sz;
	int x = 0;

	// Copy background from parent window

	RECT rc;
	GetClientRectRelative(m_hWnd, GetParent(m_hWnd), &rc);
	SetWindowOrgEx(m_mdc, rc.left, rc.top, NULL);
	SendMessage(GetParent(m_hWnd), WM_PAINT, (WPARAM)(HDC)m_mdc);
	SetWindowOrgEx(m_mdc, 0, 0, NULL);

	// Hide the menu button

	ZeroMemory(&m_buttons[m_buttons-1].rc, sizeof(RECT));

	// Draw buttons

	for (INT_PTR i = 0; i < m_buttons-1; ++i)
	{
		BUTTON *pButton = &m_buttons[i];

		// Calculate button dimensions

		GetTextExtentPoint32(m_mdc, pButton->strText + _T(" ") + pButton->strText2, 
				(int)(pButton->strText.GetLength() + 1 + pButton->strText2.GetLength()), &sz);

		pButton->rc.x = x;
		pButton->rc.y = 0;
		pButton->rc.cx = sz.cx + 2 * CB_TEXTMARGIN;
		pButton->rc.cy = cy;

		x += pButton->rc.cx;

		// If the button does not fit, hide the next ones and display the menu button

		if (pButton->rc.right > cx - CB_MENUBUTTONWIDTH)
		{
			BUTTON *pMenuButton = &m_buttons[m_buttons-1];
			pMenuButton->rc = pButton->rc;
			pMenuButton->rc.cx = CB_MENUBUTTONWIDTH;

			DestroyMenu(m_hMenu);
			m_hMenu = CreatePopupMenu();

			for (INT_PTR j = i; j < m_buttons-1; ++j)
			{
				BUTTON &button = m_buttons[j];
				ZeroMemory(&button.rc, sizeof(RECT));
				VERIFY(AppendMenu(m_hMenu, MF_STRING|(button.state == CTBBSTATE_SELECTED ? 
						MF_CHECKED : 0), j, button.strText + _T(" ") + button.strText2));
			}

			i = m_buttons-1;
			pButton = pMenuButton;
		}

		// Draw correct sprite

		if (pButton->state == CTBBSTATE_NORMAL)
			m_sprNormal.Draw(m_mdc, &pButton->rc, true);
		else if (pButton->state == CTBBSTATE_HIGHLIGHT)
			m_sprHighlight.Draw(m_mdc, pButton->rc.x - (i == 0 ? 0 : SCX(1)), pButton->rc.y, 
					pButton->rc.cx + (i == 0 ? 0 : SCX(1)), pButton->rc.cy, true);
		else if (pButton->state == CTBBSTATE_SELECTED)
			m_sprSelected.Draw(m_mdc, pButton->rc.x - (i == 0 ? 0 : SCX(1)), pButton->rc.y, 
					pButton->rc.cx + (i == 0 ? 0 : SCX(1)), pButton->rc.cy, true);

		// If menu button, draw icon

		if (i == m_buttons-1)
		{
			AlphaBlt(m_mdc, pButton->rc.x + (pButton->rc.cx - SCX(8)) / 2, (cy - SCY(8)) / 2, 
					SCX(8), SCY(8), m_mdcExpandIcon);
		}

		// Otherwise draw category text

		else
		{
			GetTextExtentPoint32(m_mdc, pButton->strText + _T(" "), (int)pButton->strText.GetLength() + 1, &sz);

			if (pButton->state == CTBBSTATE_NORMAL)
			{
				SetTextColor(m_mdc, m_clrNormalText);
				TextOut(m_mdc, pButton->rc.x + CB_TEXTMARGIN, (cy - sz.cy) / 2 - SCY(1), pButton->strText);
				SetTextColor(m_mdc, m_clrNormalText2);
				TextOut(m_mdc, pButton->rc.x + CB_TEXTMARGIN + sz.cx, (cy - sz.cy) / 2 - SCY(1), pButton->strText2);
			}
			else if (pButton->state == CTBBSTATE_HIGHLIGHT)
			{
				SetTextColor(m_mdc, m_clrHighlightText);
				TextOut(m_mdc, pButton->rc.x + CB_TEXTMARGIN, (cy - sz.cy) / 2 - SCY(1), pButton->strText);
				SetTextColor(m_mdc, m_clrHighlightText2);
				TextOut(m_mdc, pButton->rc.x + CB_TEXTMARGIN + sz.cx, (cy - sz.cy) / 2 - SCY(1), pButton->strText2);
			}
			else if (pButton->state == CTBBSTATE_SELECTED)
			{
				SetTextColor(m_mdc, m_clrSelectedText);
				TextOut(m_mdc, pButton->rc.x + CB_TEXTMARGIN, (cy - sz.cy) / 2 - SCY(1), pButton->strText);
				SetTextColor(m_mdc, m_clrSelectedText2);
				TextOut(m_mdc, pButton->rc.x + CB_TEXTMARGIN + sz.cx, (cy - sz.cy) / 2 - SCY(1), pButton->strText2);
			}
		}
	}

	Invalidate(m_hWnd);
	PostMessage(m_hWnd, WM_PAINT);
}

void CCategoryBar::SelectButton(INT_PTR nIndex, bool bToggle)
{
	if (nIndex >= m_buttons)
		{ASSERT(false); return;}

	if (nIndex == 0) // the All button is selected
	{
		foreach (m_buttons, button)
			button.state = CTBBSTATE_NORMAL;
		m_buttons[0].state = CTBBSTATE_SELECTED;
	}
	else
	{
		if (bToggle)
		{
			m_buttons[nIndex].state = (m_buttons[nIndex].state != CTBBSTATE_SELECTED ? 
					CTBBSTATE_SELECTED : CTBBSTATE_HIGHLIGHT);

			bool bCatSelected = false;
			foreach (m_buttons, button)
				if (button.state == CTBBSTATE_SELECTED)
					{bCatSelected = true; break;}
			m_buttons[0].state = (bCatSelected ? CTBBSTATE_NORMAL : CTBBSTATE_SELECTED);
		}
		else
		{
			foreach (m_buttons, but)
				but.state = CTBBSTATE_NORMAL;
			m_buttons[nIndex].state = CTBBSTATE_SELECTED;
			m_buttons[0].state = CTBBSTATE_NORMAL;
		}
	}

	FilterDB();
	
	Draw();
	Invalidate(m_hWnd);
}

void CCategoryBar::Update(bool bResetSelections /*= false*/)
{
	// Save category button selections

	RArray<bool> selections(m_buttons.GetSize());
	if (!bResetSelections)
		foreach (selections, sel, i)
			sel = (m_buttons[i].state == CTBBSTATE_SELECTED);

	// Count movies

	bool bShowSeen = GETPREFBOOL(_T("ShowSeenMovies"));
	bool bShowHidden = GETPREFBOOL(_T("ShowHiddenMovies"));
	INT_PTR nTotalCount = 0;
	INT_PTR nTVCount = 0;
	INT_PTR nMovieCount = 0;
	RArray<INT_PTR> movieCounts(GetDB()->m_categories.GetSize());
	foreach (GetDB()->m_categories, cat, i)
	{
		INT_PTR &nCategoryCount = movieCounts[i];
		nCategoryCount = 0;
		foreach (cat.directories, dir)
		{
			foreach (dir.movies, mov)
			{
				if (!bShowSeen && mov.bSeen)
					continue;
				if (!bShowHidden && mov.bHide)
					continue;

				if (mov.bType == DB_TYPE_TV)
					++nTVCount;
				else if (mov.bType)
					++nMovieCount;

				++nCategoryCount;
			}
		}
		nTotalCount += nCategoryCount;
	}

	// Array should hold autocategories(All button, Movie button, TV button), category button, and menu button

	m_buttons.SetSize(GetDB()->m_categories + m_nAutoCategories + 1);

	// Update All button

	ZeroMemory(&m_buttons[0].rc, sizeof(RECT));
	m_buttons[0].state = CTBBSTATE_NORMAL;
	m_buttons[0].strText = GETSTR(IDS_ALL);
	m_buttons[0].strText2 = _T("(") + NumberToString(nTotalCount) + _T(")");

	//Update Movie button
	if (m_nAutoCategories > CB_MOVIE_BUTTON)
	{
		ZeroMemory(&m_buttons[CB_MOVIE_BUTTON].rc, sizeof(RECT));
		m_buttons[CB_MOVIE_BUTTON].state = CTBBSTATE_NORMAL;
		m_buttons[CB_MOVIE_BUTTON].strText = GETSTR(IDS_MOVIES);
		m_buttons[CB_MOVIE_BUTTON].strText2 = _T("(") + NumberToString(nMovieCount) + _T(")");
	}

	//Update TV button

	if (m_nAutoCategories > CB_TV_BUTTON)
	{
		ZeroMemory(&m_buttons[CB_TV_BUTTON].rc, sizeof(RECT));
		m_buttons[CB_TV_BUTTON].state = CTBBSTATE_NORMAL;
		m_buttons[CB_TV_BUTTON].strText = GETSTR(IDS_TV);
		m_buttons[CB_TV_BUTTON].strText2 = _T("(") + NumberToString(nTVCount) + _T(")");
	}

	// Update category buttons

	for (INT_PTR i = m_nAutoCategories; i < m_buttons-1; ++i)
	{
		BUTTON &button = m_buttons[i];
		ZeroMemory(&button.rc, sizeof(RECT));
		button.state = CTBBSTATE_NORMAL;
		button.strText = GetDB()->m_categories[i-m_nAutoCategories].strName;
		button.strText2 = _T("(") + NumberToString(movieCounts[i-m_nAutoCategories]) + _T(")");
	}

	// Update menu button

	ZeroMemory(&m_buttons[m_buttons-1].rc, sizeof(RECT));
	m_buttons[m_buttons-1].state = CTBBSTATE_NORMAL;

	// Restore category button selections

	if (!bResetSelections)
		for (INT_PTR i = 0; i < selections && i < m_buttons; ++i)
			m_buttons[i].state = (selections[i] ? CTBBSTATE_SELECTED : CTBBSTATE_NORMAL);

	// If no category is selected, select the All button

	bool bCatSelected = false;
	for (INT_PTR i = 1; i < m_buttons; ++i)
	{
		BUTTON &button = m_buttons[i];
		if (button.state == CTBBSTATE_SELECTED)
		{bCatSelected = true; break;}
	}
	m_buttons[0].state = (bCatSelected ? CTBBSTATE_NORMAL : CTBBSTATE_SELECTED);

	// Calculate button rects

	Draw();

	// Take care of highlighting

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	//PostMessage(m_hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y)); // breaks capture
	//OnMouseMove(0, (short)pt.x, (short)pt.y);
}
