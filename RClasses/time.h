#pragma once

//
// Some important terms
// ~~~~~~~~~~~~~~~~~~~~
// 'system time'  Refers to the GMT/UTC time and that should be equal on each machine around the world, 
//                but don't count on it because the clock may not be set up properly on the machine.
// 'local time'   Is dependent on the time zone you're using so that's the time displayed by the Windows
//                clock on the taskbar.
// 'file time'    Not only used for files! It refers to the GMT/UTC time and is stored as a 64-bit value 
//                representing the number of 100-nanosecond intervals since January 1, 1601. This is the
//                preferred format since it makes calculations very easy when stored as an actual 64-bit
//                integer and not as a struct. BTW, all files on disk are stored using the GMT/UTC 'file 
//                time' and thus must be corrected to the 'local file time' to display to the user.
//
//
// List of useful functions
// ~~~~~~~~~~~~~~~~~~~~~~~~
// CompareFileTime 
// DosDateTimeToFileTime 
// FileTimeToDosDateTime 
// FileTimeToLocalFileTime 
// FileTimeToSystemTime 
// GetFileTime 
// GetLocalTime 
// GetSystemTime 
// GetSystemTimeAdjustment 
// GetSystemTimeAsFileTime 
// GetTickCount 
// GetTimeZoneInformation 
// LocalFileTimeToFileTime 
// SetFileTime 
// SetLocalTime 
// SetSystemTime 
// SetSystemTimeAdjustment 
// SetTimeZoneInformation 
// SystemTimeToFileTime 
// SystemTimeToTzSpecificLocalTime
//
//
// For more information see http://www.codeproject.com/KB/datetime/datetimedisc.aspx
// You can use _tcsftime to format time strings, see http://msdn.microsoft.com/en-us/library/fe06s4ak.aspx
//

inline bool SystemTimeToTM(const SYSTEMTIME *pST, tm *pTM)
{
	if (!pST || !pTM)
		ASSERTRETURN(false);

	pTM->tm_isdst = 0;
	pTM->tm_yday = -1; // TODO: calculate this, see http://svn.joyent.com/opensource/dtrace/ruby/ruby-1.8.5/wince/time_wce.c
	pTM->tm_wday = pST->wDayOfWeek;
	pTM->tm_year = pST->wYear - 1900;
	pTM->tm_mon = pST->wMonth - 1;
	pTM->tm_mday = pST->wDay;
	pTM->tm_hour = pST->wHour;
	pTM->tm_min = pST->wMinute;
	pTM->tm_sec = pST->wSecond;

	return true;
}

inline bool TMToSystemTime(const tm *pTM, SYSTEMTIME *pST)
{
	if (!pST || !pTM)
		ASSERTRETURN(false);

	pST->wDayOfWeek = (WORD)pTM->tm_wday;
	pST->wYear = (WORD)pTM->tm_year + 1900;
	pST->wMonth = (WORD)pTM->tm_mon + 1;
	pST->wDay = (WORD)pTM->tm_mday;
	pST->wHour = (WORD)pTM->tm_hour;
	pST->wMinute = (WORD)pTM->tm_min;
	pST->wSecond = (WORD)pTM->tm_sec;
	pST->wMilliseconds = 0;

	return true;
}

inline UINT64 SystemTimeToFileTime(const SYSTEMTIME *pST)
{
	if (!pST)
		{ASSERT(false); return 0;}
	FILETIME ft;
	SystemTimeToFileTime(pST, &ft);
	ULARGE_INTEGER n;
	n.LowPart = ft.dwLowDateTime;
	n.HighPart = ft.dwHighDateTime;
	return n.QuadPart;
}

inline bool FileTimeToSystemTime(UINT64 fileTime, SYSTEMTIME *pFT)
{
	if (!pFT)
		ASSERTRETURN(false);

	ULARGE_INTEGER n;
	n.QuadPart = fileTime;
	FILETIME ft;
	ft.dwLowDateTime = n.LowPart;
	ft.dwHighDateTime = n.HighPart;
	return FileTimeToSystemTime(&ft, pFT) != FALSE;
}

inline bool FileTimeToTM(UINT64 fileTime, tm *pTM)
{
	if (!pTM)
		ASSERTRETURN(false);
	SYSTEMTIME st;
	FileTimeToSystemTime(fileTime, &st);
	return SystemTimeToTM(&st, pTM);
}

inline UINT64 TMToFileTime(const tm *pTM)
{
	if (!pTM)
		{ASSERT(false); return 0;}
	SYSTEMTIME st;
	TMToSystemTime(pTM, &st);
	return SystemTimeToFileTime(&st);
}

inline UINT64 GetSystemTime()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	return SystemTimeToFileTime(&st);
}

inline UINT64 GetLocalTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return SystemTimeToFileTime(&st);
}

inline size_t _tcsftime(TCHAR *lpszBuffer, size_t nBufferLen, const TCHAR *lpszFormat, UINT64 fileTime)
{
	tm time;
	FileTimeToTM(fileTime, &time);
	return _tcsftime(lpszBuffer, nBufferLen, lpszFormat, &time);
}
