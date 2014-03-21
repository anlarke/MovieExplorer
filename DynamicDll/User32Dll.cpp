#include "stdafx.h"
#include "User32Dll.h"

#if WINVER < 0x0601 // only required when compiling for earlier version than Windows 7

// Function definitions

typedef BOOL (WINAPI* LPFNGETTOUCHINPUTINFO)(HTOUCHINPUT, UINT, PTOUCHINPUT, int);
typedef BOOL (WINAPI* LPFNCLOSETOUCHINPUTHANDLE)(HTOUCHINPUT);
typedef BOOL (WINAPI* LPFNREGISTERTOUCHWINDOW)(HWND, ULONG);
typedef BOOL (WINAPI* LPFNUNREGISTERTOUCHWINDOW)(HWND);
typedef BOOL (WINAPI* LPFNISTOUCHWINDOW)(HWND, PULONG);

// Global variables

HMODULE g_hUser32Dll = NULL;
User32Dll g_User32Dll;

LPFNGETTOUCHINPUTINFO		g_lpfnGetTouchInputInfo		= NULL;
LPFNCLOSETOUCHINPUTHANDLE	g_lpfnCloseTouchInputHandle	= NULL;
LPFNREGISTERTOUCHWINDOW		g_lpfnRegisterTouchWindow	= NULL;
LPFNUNREGISTERTOUCHWINDOW	g_lpfnUnregisterTouchWindow	= NULL;
LPFNISTOUCHWINDOW			g_lpfnIsTouchWindow			= NULL;

// Class implementation

User32Dll::User32Dll()
{
	g_hUser32Dll = LoadLibrary(_T("user32.dll"));
	if (!g_hUser32Dll)
		return;

	g_lpfnGetTouchInputInfo		= (LPFNGETTOUCHINPUTINFO)		GetProcAddress(g_hUser32Dll, "GetTouchInputInfo");
	g_lpfnCloseTouchInputHandle	= (LPFNCLOSETOUCHINPUTHANDLE)	GetProcAddress(g_hUser32Dll, "CloseTouchInputHandle");
	g_lpfnRegisterTouchWindow	= (LPFNREGISTERTOUCHWINDOW)		GetProcAddress(g_hUser32Dll, "RegisterTouchWindow");
	g_lpfnUnregisterTouchWindow	= (LPFNUNREGISTERTOUCHWINDOW)	GetProcAddress(g_hUser32Dll, "UnregisterTouchWindow");
	g_lpfnIsTouchWindow			= (LPFNISTOUCHWINDOW)			GetProcAddress(g_hUser32Dll, "IsTouchWindow");
}

User32Dll::~User32Dll()
{
	FreeLibrary(g_hUser32Dll);
}

// Function implementation

BOOL GetTouchInputInfo(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize)
{
	ASSERT(g_lpfnGetTouchInputInfo);
	return g_lpfnGetTouchInputInfo(hTouchInput, cInputs, pInputs, cbSize);
}

BOOL CloseTouchInputHandle(HTOUCHINPUT hTouchInput)
{
	ASSERT(g_lpfnCloseTouchInputHandle);
	return g_lpfnCloseTouchInputHandle(hTouchInput);
}

BOOL RegisterTouchWindow(HWND hwnd, ULONG ulFlags)
{
	//ASSERT(g_lpfnRegisterTouchWindow)
	if (!g_lpfnRegisterTouchWindow)
		return FALSE;
	return g_lpfnRegisterTouchWindow(hwnd, ulFlags);
}

BOOL UnregisterTouchWindow(HWND hwnd)
{
	ASSERT(g_lpfnUnregisterTouchWindow);
	return g_lpfnUnregisterTouchWindow(hwnd);
}

BOOL IsTouchWindow(HWND hwnd, PULONG pulFlags)
{
	ASSERT(g_lpfnIsTouchWindow);
	return g_lpfnIsTouchWindow(hwnd, pulFlags);
}

#endif // WINVER < 0x0601
