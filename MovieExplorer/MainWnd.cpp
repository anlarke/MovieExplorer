#include "stdafx.h"
#include "MovieExplorer.h"
#include "MainWnd.h"
#include "OptionsDlg.h"

CMainWnd::CMainWnd() : m_bShowStatusBar(false), m_bShowLog(false), m_bSizingLog(false), 
		m_nLogHeight(0), m_hFocusWnd(NULL), m_hbrBackgr(NULL)
{
	ZeroMemory(&m_ptSizing, sizeof(POINT));
	ZeroMemory(&m_rcSizing, sizeof(RECT));
}

CMainWnd::~CMainWnd()
{
}

bool CMainWnd::Create()
{
	if (!RWindow::Create<CMainWnd>(NULL, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
			0, _T("Movie Explorer"), 
			GETPREFINT(_T("MainWnd"), _T("x")), 
			GETPREFINT(_T("MainWnd"), _T("y")), 
			GETPREFINT(_T("MainWnd"), _T("cx")), 
			GETPREFINT(_T("MainWnd"), _T("cy")), NULL, NULL))
		ASSERTRETURN(false);

	bListView = true; // Start on list view.

	m_bShowStatusBar = GETPREFBOOL(_T("MainWnd"), _T("ShowStatusBar"));
	m_bShowLog = GETPREFBOOL(_T("MainWnd"), _T("ShowLog"));
	m_nLogHeight = GETPREFINT(_T("MainWnd"), _T("LogHeight"));

	// Restore window state of last session

	if (GETPREFBOOL(_T("MainWnd"), _T("Maximized")))
		ShowWindow(m_hWnd, SW_MAXIMIZE);
	else
		ShowWindow(m_hWnd, SW_SHOW);

	// Set window icon

	SendMessage(m_hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), 
			MAKEINTRESOURCE(IDI_MOVIEEXPLORER)));
	SendMessage(m_hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), 
			MAKEINTRESOURCE(IDI_MOVIEEXPLORER)));

	// Set focus to list view so we can start scrolling

	SetFocus(m_listView);

	return true;
}

LRESULT CMainWnd::WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_STATUS_WRITE)
	{
		m_statusBar.SetText((const TCHAR*)wParam);
		return 0;
	}
	else if (Msg == WM_LOG_WRITE)
	{
		m_logWnd.Write((const TCHAR*)wParam, lParam);
		return 0;
	}
	else if (Msg == WM_DBUPDATED)
	{	
		//MSG msg;
		//if (!PeekMessage(&msg, m_hWnd, WM_DBUPDATED, WM_DBUPDATED, PM_REMOVE))
		PostChildrenRec(m_hWnd, WM_DBUPDATED);
		return 0;
	}
	else if (Msg == WM_SWITCHVIEW)
	{
		// Hide other view windows

		bListView = !bListView;
		if (bListView)
			MoveWindow(m_gridView, 0, 0, 0, 0);
		else
			MoveWindow(m_listView, 0, 0, 0, 0);

		// Show new view

		RECT rc;
		GetClientRect(m_hWnd, &rc);
		OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
		PostChildrenRec(m_hWnd, WM_PAINT);

		return 0;
		//m_reBar.m_eSearch;
	}
	return RWindow::WndProc(Msg, wParam, lParam);
}

void CMainWnd::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	if (bActive && GetDB())
	{
		LOG(_T("Refreshing Database...\n"));
		GetDB()->SyncAndUpdate();
	}
}

void CMainWnd::OnActivate(WORD state, bool bMinimized, HWND hWndOther)
{
	if (state == WA_INACTIVE)
		m_hFocusWnd = GetFocus();
	else if (m_hFocusWnd)
		SetFocus(m_hFocusWnd);
}

void CMainWnd::OnClose()
{
	PostQuitMessage(0);
}

void CMainWnd::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	switch (id)
	{
		case ID_TOOLS_ZOOMIN:
		{
			SetScale(GetScale() + 0.125f);
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
			PostChildrenRec(m_hWnd, WM_SCALECHANGED);
			break;
		}
		case ID_TOOLS_ZOOMOUT:
		{
			float fScale = GetScale() - 0.125f;
			if (fScale < 0.75f)
				fScale = 0.75f;
			SetScale(fScale);
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
			PostChildrenRec(m_hWnd, WM_SCALECHANGED);
			break;
		}
		case ID_TOOLS_ZOOMRESET:
		{
			SetScale(GetSystemScale());
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
			PostChildrenRec(m_hWnd, WM_SCALECHANGED);
			break;
		}
		case ID_TOOLS_STATUSBAR:
		{
			m_bShowStatusBar = !m_bShowStatusBar;
			SETPREFBOOL(_T("MainWnd"), _T("ShowStatusBar"), m_bShowStatusBar);
			PostMessage(m_reBar, WM_COMMAND, id);
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
			break;
		}
		case ID_TOOLS_LOG:
		{
			m_bShowLog = !m_bShowLog;
			SETPREFBOOL(_T("MainWnd"), _T("ShowLog"), m_bShowLog);
			PostMessage(m_reBar, WM_COMMAND, id);
			PostMessage(m_statusBar, WM_COMMAND, id);
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
			break;
		}
		case ID_TOOLS_EXIT:
		{
			PostMessage(m_hWnd, WM_CLOSE);
			break;
		}
		case ID_TOOLS_OPTIONS:
		{
			COptionsDlg dlg;
			break;
		}
		default:
			ASSERT(false); // unknown command id
	}
}

bool CMainWnd::OnCreate(CREATESTRUCT *pCS)
{
	if (!m_reBar.Create<CReBar>(m_hWnd))
		ASSERTRETURN(false);
	if (!m_listView.Create<CListView>(m_hWnd))
		ASSERTRETURN(false);
	if (!m_gridView.Create<CGridView>(m_hWnd))
		ASSERTRETURN(false);
	if (!m_statusBar.Create<CStatusBar>(m_hWnd))
		ASSERTRETURN(false);
	if (!m_logWnd.Create<CLogWnd>(m_hWnd))
		ASSERTRETURN(false);

	OnPrefChanged();

	return true;
}

void CMainWnd::OnDestroy()
{
	DeleteObject(m_hbrBackgr);
}

void CMainWnd::OnLButtonDown(DWORD keys, short x, short y)
{
	RECT rcLog;
	GetWindowRect(m_logWnd, &rcLog);
	POINT pt;
	GetCursorPos(&pt);

	if (pt.y > rcLog.top - SCY(3) && pt.y < rcLog.top + SCY(3))
		m_bSizingLog = true;
}

void CMainWnd::OnLButtonUp(DWORD keys, short x, short y)
{
	m_bSizingLog = false;
}

void CMainWnd::OnMouseMove(DWORD keys, short x, short y)
{
	RECT rcLog;
	GetWindowRect(m_logWnd, &rcLog);
	POINT pt;
	GetCursorPos(&pt);

	if (m_bSizingLog)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		m_nLogHeight = m_rcSizing.bottom - m_rcSizing.top - pt.y + m_ptSizing.y;
		if (m_nLogHeight <= 0)
		{
			ReleaseCapture();
			m_nLogHeight = 108;
			OnCommand(ID_TOOLS_LOG, 0, NULL);
			m_bSizingLog = false;
		}
		else if (m_nLogHeight > rc.bottom - S(80))
			m_nLogHeight = rc.bottom - S(80);
		SETPREFINT(_T("MainWnd"), _T("LogHeight"), m_nLogHeight);
		OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
		PostMessage(m_hWnd, WM_PAINT);
		//PostChildren(m_hWnd, WM_PAINT);
		return;
	}

	if (!(pt.y > rcLog.top - SCY(3) && pt.y < rcLog.top + SCY(3)))
		ReleaseCapture();
}

void CMainWnd::OnMove(short x, short y)
{
	SaveWindowPos();
}

void CMainWnd::OnPrefChanged()
{
	PostMessage(*GetDB(), WM_PREFCHANGED);

	DeleteObject(m_hbrBackgr);
	m_hbrBackgr = CreateSolidBrush(GETTHEMECOLOR(_T("ListView"), _T("BackgrColor")));
	SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)m_hbrBackgr);

	PostChildrenRec(m_hWnd, WM_PREFCHANGED);
}

bool CMainWnd::OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg)
{
	// The WM_SETCURSOR message is first send to the parent window (this window), so this is the 
	// place where we can discover if the mouse is above a movable border. If it is, capture the 
	// mouse input so we can handle sizing.

	RECT rcLog;
	GetWindowRect(m_logWnd, &rcLog);
	POINT pt;
	GetCursorPos(&pt);

	if (m_bShowLog && pt.y > rcLog.top - SCY(3) && pt.y < rcLog.top + SCY(3))
	{
		m_ptSizing = pt;
		m_rcSizing = rcLog;
		SetCapture(m_hWnd);
		return SetCursor(LoadCursor(NULL, IDC_SIZENS)) != NULL;
	}

	return RWindow::OnSetCursor(hWnd, hitTest, mouseMsg);
}

void CMainWnd::OnSetFocus(HWND hWndLoseFocus)
{
	if (bListView)
		SetFocus(m_listView);
	else
		SetFocus(m_gridView);
}

void CMainWnd::OnSize(DWORD type, WORD cx, WORD cy)
{
	SaveWindowPos();

	RECT rcReBar = {0, 0, cx, m_reBar.GetHeight()};
	RECT rcLog = {0, (m_bShowLog ? cy - m_nLogHeight : cy), cx, cy};
	RECT rcStatusBar = {0, (m_bShowStatusBar ? rcLog.top - SCY(23) : rcLog.top), cx, rcLog.top};
	RECT rcList = {0, rcReBar.bottom, cx, rcStatusBar.top};

	MoveWindow(m_reBar, &rcReBar);
	MoveWindow(m_logWnd, &rcLog);
	MoveWindow(m_statusBar, &rcStatusBar);

	if (bListView)
		MoveWindow(m_listView, &rcList);
	else
		MoveWindow(m_gridView, &rcList);
}

void CMainWnd::SaveWindowPos()
{
	if (!IsWindow(m_hWnd))
		{ASSERT(false); return;}

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(m_hWnd, &wp);

	if (wp.showCmd == SW_SHOWMAXIMIZED)
		SETPREFBOOL(_T("MainWnd"), _T("Maximized"), true);
	else
	{
		RECT rc;
		GetWindowRect(m_hWnd, &rc);
			
		SETPREFINT(_T("MainWnd"), _T("x"), rc.left);
		SETPREFINT(_T("MainWnd"), _T("y"), rc.top);
		SETPREFINT(_T("MainWnd"), _T("cx"), rc.right - rc.left);
		SETPREFINT(_T("MainWnd"), _T("cy"), rc.bottom - rc.top);
		SETPREFBOOL(_T("MainWnd"), _T("Maximized"), false);
	}
}
