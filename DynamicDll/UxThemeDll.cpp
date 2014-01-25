#include "stdafx.h"
#include "UxThemeDll.h"

// Function definitions

typedef BOOL (WINAPI* LPFNISAPPTHEMED)();
typedef HTHEME (WINAPI* LPFNOPENTHEMEDATA)(HWND, LPCWSTR);
typedef HRESULT (WINAPI* LPFNCLOSETHEMEDATA)(HTHEME);
typedef HRESULT (WINAPI* LPFNDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT*, const RECT*);
typedef HRESULT (WINAPI* LPFNSETWINDOWTHEME)(HWND, LPCWSTR, LPCWSTR);
typedef HRESULT (WINAPI* LPFNGETTHEMEMETRIC)(HTHEME, HDC, int, int, int, int*);

// Global variables

HMODULE g_hUxThemeDll = NULL;
UxThemeDll g_UxThemeDll;

LPFNISAPPTHEMED g_lpfnIsAppThemed;
LPFNOPENTHEMEDATA g_lpfnOpenThemeData;
LPFNCLOSETHEMEDATA g_lpfnCloseThemeData;
LPFNDRAWTHEMEBACKGROUND g_lpfnDrawThemeBackground;
LPFNSETWINDOWTHEME g_lpfnSetWindowTheme;
LPFNGETTHEMEMETRIC g_lpfnGetThemeMetric;

// Class implementation

UxThemeDll::UxThemeDll()
{
	g_hUxThemeDll = LoadLibrary(_T("uxtheme.dll"));
	if (!g_hUxThemeDll)
		return;

	g_lpfnIsAppThemed = (LPFNISAPPTHEMED)GetProcAddress(g_hUxThemeDll, "IsAppThemed");
	g_lpfnOpenThemeData = (LPFNOPENTHEMEDATA)GetProcAddress(g_hUxThemeDll, "OpenThemeData");
	g_lpfnCloseThemeData = (LPFNCLOSETHEMEDATA)GetProcAddress(g_hUxThemeDll, "CloseThemeData");
	g_lpfnDrawThemeBackground = (LPFNDRAWTHEMEBACKGROUND)GetProcAddress(g_hUxThemeDll, "DrawThemeBackground");
	g_lpfnSetWindowTheme = (LPFNSETWINDOWTHEME)GetProcAddress(g_hUxThemeDll, "SetWindowTheme");
	g_lpfnGetThemeMetric = (LPFNGETTHEMEMETRIC)GetProcAddress(g_hUxThemeDll, "GetThemeMetric");
}

UxThemeDll::~UxThemeDll()
{
	FreeLibrary(g_hUxThemeDll);
}

// Function implementation

BOOL IsAppThemed()
{
	if (!g_hUxThemeDll)
		return FALSE;
	ASSERT(g_lpfnIsAppThemed);
	return g_lpfnIsAppThemed();
}

HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	ASSERT(g_lpfnOpenThemeData); 
	return g_lpfnOpenThemeData(hwnd, pszClassList);
}

HRESULT CloseThemeData(HTHEME hTheme)
{
	ASSERT(g_lpfnCloseThemeData);
	return g_lpfnCloseThemeData(hTheme);
}

HRESULT DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect)
{
	ASSERT(g_lpfnDrawThemeBackground);
	return g_lpfnDrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

HRESULT SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList)
{
	ASSERT(g_lpfnSetWindowTheme);
	return g_lpfnSetWindowTheme(hwnd, pszSubAppName, pszSubIdList);
}

HRESULT GetThemeMetric(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int *piVal)
{
	ASSERT(g_lpfnGetThemeMetric);
	return g_lpfnGetThemeMetric(hTheme, hdc, iPartId, iStateId, iPropId, piVal);
}
