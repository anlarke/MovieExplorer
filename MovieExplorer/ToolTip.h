#pragma once

#include "stdafx.h"
#include "MovieExplorer.h"

inline BOOL CreateToolTip(HWND hwnd, int id, HINSTANCE hInst, const TCHAR *strTooltipText)
{
	TOOLINFO toolInfo;
	HWND hTtip;
	hTtip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, \
		CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hInst, NULL);
	toolInfo.cbSize = sizeof (toolInfo)-4;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = 0;
	toolInfo.lpszText = NULL;
	toolInfo.hinst = hInst;
	toolInfo.hwnd = hwnd;
	toolInfo.uId = (UINT)GetDlgItem(hwnd, id);
	toolInfo.lpszText = (LPWSTR)strTooltipText;
	SendMessage(hTtip, (UINT)TTM_ADDTOOL, (WPARAM)0, (LPARAM)&toolInfo);
	return 0;
}
