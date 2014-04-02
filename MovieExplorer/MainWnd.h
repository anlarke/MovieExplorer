#pragma once

#include "ReBar.h"
#include "ListView.h"
#include "GridView.h"
#include "StatusBar.h"
#include "LogWnd.h"

class CMainWnd : public RWindow
{
	friend class RWindow;

public:
	CMainWnd();
	~CMainWnd();
	bool Create();

protected:
	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	void OnActivate(WORD state, bool bMinimized, HWND hWndOther);
	void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	void OnClose();
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);
	bool OnCreate(CREATESTRUCT *pCS);
	void OnDestroy();
	void OnLButtonDown(DWORD keys, short x, short y);
	void OnLButtonUp(DWORD keys, short x, short y);
	void OnMouseMove(DWORD keys, short x, short y);
	void OnMove(short x, short y);
	void OnPrefChanged();
	bool OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg);
	void OnSetFocus(HWND hWndLoseFocus);
	void OnSize(DWORD type, WORD cx, WORD cy);

	void SaveWindowPos();

	CReBar m_reBar;
	CListView m_listView;
	CGridView m_gridView;
	CStatusBar m_statusBar;
	CLogWnd m_logWnd;
	bool m_bShowStatusBar, m_bShowLog, m_bSizingLog, m_bListView; 
	int m_nLogHeight;
	POINT m_ptSizing;
	RECT m_rcSizing;
	HWND m_hFocusWnd;
	HBRUSH m_hbrBackgr;
};
