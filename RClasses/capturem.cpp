#include "stdafx.h"
#include "capturem.h"

#define CAPTHR_WAIT 5000

HANDLE g_hCapThr = NULL;
UINT g_idCapThr = 0;
RArray<HWND> g_handles;
RCriticalSection g_sect;
REvent g_eQuit;

UINT CALLBACK CapThr(void *pParam)
{
	POINT ptScreen, ptScreenPrev = {0, 0}, ptClient;
	INT_PTR i;
	HWND hWnd;

	while (WaitForSingleObject(g_eQuit, 10) == WAIT_TIMEOUT)
	{
		RLock lock(&g_sect);
		if (g_handles)
		{
			GetCursorPos(&ptScreen);
			if (ptScreen.x != ptScreenPrev.x || ptScreen.y != ptScreenPrev.y)
			{
				for (i = 0; i < g_handles; ++i)
				{
					hWnd = g_handles[i];
					ptClient = ptScreen;
					ScreenToClient(hWnd, &ptClient);
					PostMessage(hWnd, WM_CAPTUREM, ptClient.x, ptClient.y);
				}
				ptScreenPrev = ptScreen;
			}
		}
	}
	return 0;
}

void InitCaptureM()
{
	g_hCapThr = (HANDLE)_beginthreadex(NULL, 0, CapThr, NULL, 0, &g_idCapThr);
	ASSERT(g_hCapThr);
}

void UninitCaptureM()
{
	g_eQuit.SetEvent();
	VERIFY(WaitForSingleObject(g_hCapThr, CAPTHR_WAIT) == WAIT_OBJECT_0);
	g_hCapThr = NULL;
	g_idCapThr = 0;
}

void SetCaptureM(HWND hWnd)
{
	RLock lock(&g_sect);
	if (g_handles.IndexOf(hWnd) == -1)
		g_handles.Add(hWnd);
}

bool ReleaseCaptureM(HWND hWnd)
{
	RLock lock(&g_sect);
	for (INT_PTR i = 0; i < g_handles; ++i)
		if (g_handles[i] == hWnd)
			{g_handles.RemoveAt(i); return true;}
	return false;
}
