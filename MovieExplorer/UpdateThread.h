#pragma once

struct UPDATETHREADDATA
{
	HWND hDatabaseWnd;
	REvent eReady;
};

UINT CALLBACK UpdateThread(void *pParam);
