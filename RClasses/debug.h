//
// IMPORTANT: Include this file in your 'stdafx.h' like this (otherwise line numbers 
// will not be displayed):
//
// #include <windows.h>
// #include <stdio.h>
// #include <tchar.h>
// #include "debug.h" <-- this file
// #include <new>
// #include ...
//
// 
// To use the 'placement new' operator do it like this:
// 
// #include <new>
// RString *pString = (RString*)malloc(sizeof(RString));
// #pragma push_macro("new")
// #undef new
// new(pString) RString;
// #pragma pop_macro("new")
//

#pragma once

#pragma warning(disable:4996)			// 'strnicmp': The POSIX name for this item is deprecated.
#pragma warning(disable:4800)			// 'BOOL' : forcing value to bool 'true' or 'false'

//
// Enable memory leak detection
//

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DBG_NEW new (_NORMAL_BLOCK , __FILE__, __LINE__)
	#define new DBG_NEW
	#define DEBUG_INIT() {_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);}
#else
	#include <stdlib.h>
	#define DEBUG_INIT() __noop
#endif

//
// Implement macro's ASSERT and TRACE
//

#ifdef _DEBUG
	#define ASSERT _ASSERT
	#define VERIFY _ASSERT
	
	#if (_MSC_VER < 1600)
		#define ASSERTRETURN(x) {BOOST_AUTO(r, x); ASSERT(r); return r;}
	#else
		#define ASSERTRETURN(x) {auto r = x; ASSERT(r); return r;}
	#endif

	inline void Trace(const TCHAR *lpszFormat, ...)
	{
		va_list args;            
		va_start(args, lpszFormat);
		TCHAR buffer[2048];
		int n = _vsntprintf_s(buffer, 2048, _TRUNCATE, lpszFormat, args);
		va_end(args);

		if (n == -1)
		{
			OutputDebugString(_T("Trace buffer overflow!\n"));
			//ASSERT(false);
		}
		else
			OutputDebugString(buffer);
	}

	#define TRACE Trace
	#define TRACE0(x) OutputDebugString(x)
#else
	#define ASSERT(...) __noop
	#define VERIFY(x) x
	#define ASSERTRETURN(x) return x
	#define TRACE(...) __noop
	#define TRACE0(...) __noop
#endif

//
// Performance tools
//

inline void MeasureTime(bool bStart, const TCHAR *lpszText)
{
	static INT64 start;
	if (bStart)
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
	else
	{
		INT64 end, freq;
		QueryPerformanceCounter((LARGE_INTEGER*)&end);
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		
		#ifdef _DEBUG
			if (lpszText)
				_tprintf(_T("%s %.2f ms\n"), lpszText, (double)(1000 * (end - start)) / freq);
			else
				_tprintf(_T("%.2f ms\n"), (double)(1000 * (end - start)) / freq);
		#else
			if (lpszText)
			{
				TCHAR buf[128];
				_stprintf(buf, _T("%s %.3f ms\n"), lpszText, (double)(1000 * (end - start)) / freq);
				//MessageBox(NULL, buf, NULL, 0);
				_tprintf(buf);
			}
			else
			{
				TCHAR buf[128];
				_stprintf(buf, _T("%.3f ms\n"), (double)(1000 * (end - start)) / freq);
				//MessageBox(NULL, buf, NULL, 0);
				_tprintf(buf);
			}
		#endif
	}
}

inline void tic()
	{MeasureTime(true, NULL);}

inline void toc(const TCHAR *lpszText = NULL)
	{MeasureTime(false, lpszText);}

#define MT_START() MeasureTime(true, NULL)
#define MT_END(x) MeasureTime(false, x)
