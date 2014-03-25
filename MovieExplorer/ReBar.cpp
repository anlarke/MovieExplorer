#include "stdafx.h"
#include "MovieExplorer.h"
#include "ReBar.h"
#include "ToolTip.h"

#define BUTTON_ID_TOOLS			10
#define BUTTON_ID_SEENMAIN		11
#define BUTTON_ID_SORT			12

CReBar::CReBar() : m_hToolsMenu(NULL), m_hSortMenu(NULL)
{
}

CReBar::~CReBar()
{
}

int CReBar::GetHeight()
{
	return SCY(32);
}

void CReBar::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	switch (id)
	{
	case ID_TOOLS:
		{
		// If the button is clicked again while menu is shown, we shouldn't show it again

		static bool bIgnoreNext = false;
		if (hWndControl == (HWND)-1)
		{
			bIgnoreNext = true;
			return;
		}
		if (bIgnoreNext)
		{
			bIgnoreNext = false;
			return;
		}

		// Install a hook to capture mouse events when the menu is shown

		HHOOK hHook = SetWindowsHookEx(WH_MOUSE_LL, LLMouseProc, GetModuleHandle(NULL), 0);
		ASSERT(hHook);
		LLMouseProc(-1, -1, (LPARAM)hHook);
		LLMouseProc(-2, -2, (LPARAM)this);

		// Show menu, make sure commands get sent to the main window

		RECT rc;
		GetWindowRect(m_btnTools, &rc);

		VERIFY(TrackPopupMenu(m_hToolsMenu, TPM_RIGHTALIGN|TPM_TOPALIGN, rc.right, rc.bottom, 
				0, GetTopLevelParent(m_hWnd), NULL));

		UnhookWindowsHookEx(hHook);

		// Restore button state

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_btnTools, &pt);

		PostMessage(m_btnTools, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
		}
		break;
	case ID_TOOLS_STATUSBAR:
		CheckMenuItem(m_hToolsMenu, ID_TOOLS_STATUSBAR, 
				(GETPREFBOOL(_T("MainWnd"), _T("ShowStatusBar")) ? MF_CHECKED : MF_UNCHECKED));
		break;
	case ID_TOOLS_LOG:
		CheckMenuItem(m_hToolsMenu, ID_TOOLS_LOG, 
				(GETPREFBOOL(_T("MainWnd"), _T("ShowLog")) ? MF_CHECKED : MF_UNCHECKED));
		break;
	case ID_SEEN:
		m_btnSeen.SetCheck(!m_btnSeen.GetCheck());
		SETPREFBOOL(_T("ShowSeenMovies"), m_btnSeen.GetCheck());
		SendMessage(*GetDB(), WM_PREFCHANGED);
		GetDB()->Filter();
		break;
	case ID_SORT:
		{
		// If the button is clicked again while menu is shown, we shouldn't show it again

		static bool bIgnoreNext = false;
		if (hWndControl == (HWND)-1)
		{
			bIgnoreNext = true;
			return;
		}
		if (bIgnoreNext)
		{
			bIgnoreNext = false;
			return;
		}

		// Install a hook to capture mouse events when the menu is shown

		HHOOK hHook = SetWindowsHookEx(WH_MOUSE_LL, LLMouseProc, GetModuleHandle(NULL), 0);
		ASSERT(hHook);
		LLMouseProc(-1, -1, (LPARAM)hHook);
		LLMouseProc(-2, -2, (LPARAM)this);

		// Show menu, make sure commands get sent to THIS window

		RECT rc;
		GetWindowRect(m_btnSort, &rc);

		VERIFY(TrackPopupMenu(m_hSortMenu, TPM_RIGHTALIGN|TPM_TOPALIGN, rc.right, rc.bottom, 
				0, m_hWnd, NULL));

		UnhookWindowsHookEx(hHook);

		// Restore button state

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_btnSort, &pt);

		PostMessage(m_btnSort, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
		}
		break;
	case ID_SEARCH:
		if (notifyCode == EN_RETURN || (m_bInstantSearch && notifyCode == EN_CHANGE))
		{
			RString strText = m_eSearch.GetText();
			RObArray<RString> keywords;

			if (m_bLiteralSearch)
			{
				if (!strText.IsEmpty())
					keywords.Add(strText);
			}
			else
			{
				RRegEx re(_T("(?:\"([^\"]+)\")|([^ ]+)"));
				if (!strText.IsEmpty())
					keywords.Add(strText);
				while (re.Search(strText))
				{
					RString strMatch;
					for (INT_PTR i = 1; i < re.GetMatchCount() && strMatch.IsEmpty(); ++i)
						strMatch = re.GetMatch(i);
					strMatch.Replace(_T("\""), _T(""));
					keywords.Add(strMatch);
					strText.Replace(re.GetMatch(0), _T(""));
				}
			}
			GetDB()->FilterByKeywords(keywords);
		}
		break;
	case ID_SORT_TITLEASC:
	case ID_SORT_TITLEDES:
	case ID_SORT_YEARASC:
	case ID_SORT_YEARDES:
	case ID_SORT_RATINGASC:
	case ID_SORT_RATINGDES:
	case ID_SORT_FILETIMEASC:
	case ID_SORT_FILETIMEDES:
		SETPREFINT(_T("SortBy"), id - ID_SORT_TITLEASC);
		CheckMenuRadioItem(m_hSortMenu, ID_SORT_TITLEASC, ID_SORT_FILETIMEDES, id, MF_CHECKED);
		PostMessage(*GetDB(), WM_PREFCHANGED);
		PostMessage(m_hWnd, WM_COMMAND, MAKEWPARAM(ID_SEARCH, EN_RETURN), (LPARAM)(HWND)m_eSearch);
		break;
	default:
		ASSERT(false);
	}
}

bool CReBar::OnCreate(CREATESTRUCT *pCS)
{
	if (!m_btnTools.Create<CToolBarButton>(m_hWnd, ID_TOOLS, &m_mdcToolsBtn, NULL, true, (HMENU)BUTTON_ID_TOOLS))
		ASSERTRETURN(false);

	if (!m_btnSeen.Create<CToolBarButton>(m_hWnd, ID_SEEN, &m_mdcSeenBtn, NULL, false, (HMENU)BUTTON_ID_SEENMAIN))
		ASSERTRETURN(false);

	if (!m_btnSort.Create<CToolBarButton>(m_hWnd, ID_SORT, &m_mdcSortBtn, NULL, true, (HMENU)BUTTON_ID_SORT))
		ASSERTRETURN(false);

	if (!m_eSearch.Create<RHintEdit>(m_hWnd, ES_AUTOHSCROLL, 0, NULL, 0, 0, 0, 0, ID_SEARCH))
		ASSERTRETURN(false);

	if (!m_categoryBar.Create<CCategoryBar>(m_hWnd))
		ASSERTRETURN(false);

	// Create tooltip windows for buttons

	HINSTANCE hInst = GetModuleHandle(NULL);
	CreateToolTip(m_hWnd, BUTTON_ID_TOOLS, hInst, GETSTR(IDS_TOOLTIP_TOOLS));
	CreateToolTip(m_hWnd, BUTTON_ID_SEENMAIN, hInst, GETSTR(IDS_TOOLTIP_SEENMAIN));
	CreateToolTip(m_hWnd, BUTTON_ID_SORT, hInst, GETSTR(IDS_TOOLTIP_SORT));

	m_mdc.Create(0, 0);
	OnPrefChanged();

	return true;
}

HBRUSH CReBar::OnCtlColorEdit(HDC hDC, HWND hWnd)
{
	if (hWnd == m_eSearch)
		return m_eSearch.ReflectCtlColorEdit(hDC, hWnd);
	return RWindow::OnCtlColorEdit(hDC, hWnd);
}

void CReBar::OnDestroy()
{
	DestroyMenu(m_hToolsMenu);
}

void CReBar::OnPaint(HDC hDC)
{
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	BitBlt(hDC, &rc, m_mdc);
}

void CReBar::OnPrefChanged()
{
	m_bBackgrWinDefault = GETTHEMEBOOL(_T("ReBar"), _T("BackgrWindowsDefault"));
	m_clrBackgrT = GETTHEMECOLOR(_T("ReBar"), _T("BackgrTopColor"));
	m_clrBackgrB = GETTHEMECOLOR(_T("ReBar"), _T("BackgrBottomColor"));
	m_clrBackgrLine = GETTHEMECOLOR(_T("ReBar"), _T("BackgrLineColor"));
	
	m_bInstantSearch = GETPREFBOOL(_T("Search"), _T("Instantly"));
	m_bLiteralSearch = GETPREFBOOL(_T("Search"), _T("Literally"));

	m_mdcToolsBtn.Create(48, 48);
	DrawAlphaMap(m_mdcToolsBtn, 0, 0, IDA_WRENCH, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	m_mdcSeenBtn.Create(48, 48);
	DrawAlphaMap(m_mdcSeenBtn, 0, 0, IDA_SEEN, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	m_mdcSortBtn.Create(48, 48);
	DrawAlphaMap(m_mdcSortBtn, 0, 0, IDA_SORT, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	// Search box

	//GETTHEMEFONT(&m_fntSearch, _T("SearchBox"), _T("Font"));
	//m_eSearch.SetFont(m_fntSearch);
	m_strSearchBoxFont = GETTHEMESTR(_T("SearchBox"),  _T("Font"));
	m_fSearchBoxFontSize = GETTHEMEFLOAT(_T("SearchBox"),  _T("FontSize"));

	m_eSearch.SetHint(GETSTR(IDS_SEARCH));
	m_eSearch.SetHintColor(GETTHEMECOLOR(_T("SearchBox"), _T("HintColor")));
	m_eSearch.SetTextColor(GETTHEMECOLOR(_T("SearchBox"), _T("TextColor")));
	m_eSearch.SetBackgrColor(GETTHEMECOLOR(_T("SearchBox"), _T("BackgrColor")));

	int r = SCX(GETTHEMEINT(_T("SearchBox"), _T("BorderRadius")));
	m_sprSearchBox.Create(SCX(23), SCX(23), r+1, r+1, r+1, r+1);
	DrawRoundedRect(m_sprSearchBox, 0, 0, SCX(23), SCX(23), r,
			GETTHEMECOLOR(_T("SearchBox"), _T("BorderColor")),
			GETTHEMEALPHA(_T("SearchBox"), _T("BorderAlpha")), false);
	DrawRoundedRect(m_sprSearchBox, SCX(1), SCX(1), SCX(23) - 2 * SCX(1), SCX(23) - 2 * SCX(1), 
			(r > 0 ? r - SCX(1) : 0),
			GETTHEMECOLOR(_T("SearchBox"), _T("BackgrColor")));

	// Tools menu

	DestroyMenu(m_hToolsMenu);

	m_hToolsMenu = CreatePopupMenu();
	AppendMenu(m_hToolsMenu, MF_STRING, ID_TOOLS_ZOOMIN, GETSTR(ID_TOOLS_ZOOMIN));
	AppendMenu(m_hToolsMenu, MF_STRING, ID_TOOLS_ZOOMOUT, GETSTR(ID_TOOLS_ZOOMOUT));
	AppendMenu(m_hToolsMenu, MF_STRING, ID_TOOLS_ZOOMRESET, GETSTR(ID_TOOLS_ZOOMRESET));
	AppendMenu(m_hToolsMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(m_hToolsMenu, MF_STRING, ID_TOOLS_STATUSBAR, GETSTR(ID_TOOLS_STATUSBAR));
	AppendMenu(m_hToolsMenu, MF_STRING, ID_TOOLS_LOG, GETSTR(ID_TOOLS_LOG));
	AppendMenu(m_hToolsMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(m_hToolsMenu, MF_STRING, ID_TOOLS_OPTIONS, GETSTR(ID_TOOLS_OPTIONS));
	AppendMenu(m_hToolsMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(m_hToolsMenu, MF_STRING, ID_TOOLS_EXIT, GETSTR(ID_TOOLS_EXIT));

	if (GETPREFBOOL(_T("MainWnd"), _T("ShowStatusBar")))
		CheckMenuItem(m_hToolsMenu, ID_TOOLS_STATUSBAR, MF_CHECKED);
	if (GETPREFBOOL(_T("MainWnd"), _T("ShowLog")))
		CheckMenuItem(m_hToolsMenu, ID_TOOLS_LOG, MF_CHECKED);

	// Check seen button

	m_btnSeen.SetCheck(GETPREFBOOL(_T("ShowSeenMovies")));

	// Sort menu

	m_hSortMenu = CreatePopupMenu();
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_TITLEASC, GETSTR(ID_SORT_TITLEASC));
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_TITLEDES, GETSTR(ID_SORT_TITLEDES));
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_YEARASC, GETSTR(ID_SORT_YEARASC));
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_YEARDES, GETSTR(ID_SORT_YEARDES));
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_RATINGASC, GETSTR(ID_SORT_RATINGASC));
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_RATINGDES, GETSTR(ID_SORT_RATINGDES));
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_FILETIMEASC, GETSTR(ID_SORT_FILETIMEASC));
	AppendMenu(m_hSortMenu, MF_STRING, ID_SORT_FILETIMEDES, GETSTR(ID_SORT_FILETIMEDES));

	CheckMenuRadioItem(m_hSortMenu, ID_SORT_TITLEASC, ID_SORT_FILETIMEDES, 
			GETPREFINT(_T("SortBy")) + ID_SORT_TITLEASC, MF_CHECKED);

	OnScaleChanged();
}

void CReBar::OnScaleChanged()
{
	m_fntSearch.Create(m_strSearchBoxFont, m_fSearchBoxFontSize);
	m_eSearch.SetFont(m_fntSearch);

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	OnSize(0, (WORD)rc.right, (WORD)rc.bottom);

	Draw();
}

void CReBar::OnSize(DWORD type, WORD cx, WORD cy)
{
	VERIFY(m_mdc.Create(cx, cy));

	Draw();

	if (cx == 0 || cy == 0)
	{
		MoveWindow(m_btnTools, 0, 0, 0, 0);
		MoveWindow(m_categoryBar, 0, 0, 0, 0);
		MoveWindow(m_eSearch, 0, 0, 0, 0);
	}
	else
	{
		MoveWindow(m_btnTools, cx - SCX(4) - SCX(24), (cy - SCX(24)) / 2, SCX(24), SCX(24));
		MoveWindow(m_btnSeen, cx - 2*SCX(4) - 2*SCX(24), (cy - SCX(24)) / 2, SCX(24), SCX(24));
		MoveWindow(m_btnSort, cx - 3*SCX(4) - 3*SCX(24), (cy - SCX(24)) / 2, SCX(24), SCX(24));
		MoveWindow(m_categoryBar, 0, 0, cx - SCX(296), cy);
		MoveWindow(m_eSearch, cx - SCX(285), SCY(7), SCX(195), SCY(19), TRUE);
		//MoveWindow(m_categoryBar, 0, 0, cx - SCX(268), cy);
		//MoveWindow(m_eSearch, cx - SCX(257), SCY(7), SCX(195), SCY(19), TRUE);
	}

	PostMessage(m_hWnd, WM_PAINT);
	PostChildren(m_hWnd, WM_PAINT);
}

void CReBar::Draw()
{
	int cx, cy;
	VERIFY(m_mdc.GetDimensions(&cx, &cy));

	// Draw background

	if (m_bBackgrWinDefault)
	{
		if (IsAppThemed())
		{
			HTHEME hTheme = OpenThemeData(m_hWnd, L"REBAR");
			RECT rc = {0, 0, cx, cy};
			DrawThemeBackground(hTheme, m_mdc, RP_BACKGROUND, 0, &rc, NULL);
			CloseThemeData(hTheme);
		}
		else
		{
			DrawRect(m_mdc, 0, 0, cx, cy, GetSysColor(COLOR_3DFACE));
			DrawRect(m_mdc, 0, cy - SCY(1), cx, SCY(1), GetSysColor(COLOR_3DSHADOW));
		}
	}
	else
	{
		DrawRectGradientV(m_mdc, 0, 0, cx, cy - SCY(1), m_clrBackgrT, m_clrBackgrB);
		DrawRect(m_mdc, 0, cy - SCY(1), cx, SCY(1), m_clrBackgrLine);
	}

	// Draw rounded background and border around search box

	m_sprSearchBox.Draw(m_mdc, cx - SCX(287), SCY(4), SCX(199), SCY(23), true);
	//m_sprSearchBox.Draw(m_mdc, cx - SCX(259), SCY(4), SCX(199), SCY(23), true);

	Invalidate(m_hWnd);
	PostMessage(m_hWnd, WM_PAINT);
}

LRESULT CALLBACK CReBar::LLMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// Store information passed to the procedure

	static HHOOK hhk = NULL;
	if (nCode == -1 && wParam == -1)
	{
		hhk = (HHOOK)lParam;
		return 0;
	}

	static CReBar *pReBar = NULL;
	if (nCode == -2 && wParam == -2)
	{
		pReBar = (CReBar*)lParam;
		return 0;
	}

	// Handle mouse messages

	if (wParam == WM_LBUTTONDOWN)
	{
		POINT pt;
		GetCursorPos(&pt);
		//ScreenToClient(pCategoryBar->m_hWnd, &pt);
		//
		//if (PtInRect(&pCategoryBar->m_rcToolsBtn, pt))
		//{
		//	PostMessage(pCategoryBar->m_hWnd, WM_LBUTTONDOWN, -1, -1); // ignore next click
		//	PostMessage(GetTopLevelParent(pCategoryBar->m_hWnd), WM_CANCELMODE, 0, 0);
		//}

		RECT rc;
		GetWindowRect(pReBar->m_btnTools, &rc);
		if (PtInRect(&rc, pt))
		{
			PostMessage(pReBar->m_hWnd, WM_COMMAND, ID_TOOLS, -1); // ignore next click
			PostMessage(GetTopLevelParent(pReBar->m_hWnd), WM_CANCELMODE, 0, 0);
		}

		GetWindowRect(pReBar->m_btnSort, &rc);
		if (PtInRect(&rc, pt))
		{
			PostMessage(pReBar->m_hWnd, WM_COMMAND, ID_SORT, -1); // ignore next click
			PostMessage(GetTopLevelParent(pReBar->m_hWnd), WM_CANCELMODE, 0, 0);
		}
	}

	return CallNextHookEx(hhk, nCode, wParam, lParam);
}
