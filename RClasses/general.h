#pragma once

#define DUX(n)		DialogUnitsToPixels(n, true)
#define DUY(n)		DialogUnitsToPixels(n, false)

#define CHARSET_AUTODETECT	0
#define CHARSET_ANSI		1
#define CHARSET_UTF16LE		2
#define CHARSET_UTF16BE		3
#define CHARSET_UTF8		4
#define CHARSET_UTF16		CHARSET_UTF16LE

#include <strsafe.h>

inline RString GetAppPath()
{
	RString str;
	GetModuleFileName(NULL, str.GetBuffer(_MAX_PATH), _MAX_PATH);
	str.ReleaseBuffer();
	return str;
}

inline RString GetAppDir()
{
	RString str(GetAppPath());
	return str.Left(str.ReverseFind(_T('\\')));
}

inline RString CorrectPath(RString strPath, bool bResolveDots = false)
{
	strPath.Trim();

	bool bNormalPath = strPath.Find(_T(':')) != -1;
	bool bNetworkPath = (strPath.GetLength() >= 2 && strPath.Left(2) == _T("\\\\"));

	if (!bNormalPath && !bNetworkPath) // relative path
	{
		if (strPath.IsEmpty())
			strPath = GetAppDir();
		else
			strPath = GetAppDir() + _T("\\") + strPath; 
	}

	bNetworkPath = (strPath.Left(2) == _T("\\\\"));

	strPath.Replace(_T("/"), _T("\\"));

	if (bNetworkPath)
	{
		strPath = strPath.Mid(2);
		while (strPath.Replace(_T("\\\\"), _T("\\")));
		strPath = _T("\\\\") + strPath;
	}
	else
		while (strPath.Replace(_T("\\\\"), _T("\\")));

	if (bResolveDots)
	{
		while (strPath.Replace(_T("\\.\\"), _T("\\")));

		INT_PTR m, n;
		while ((n = strPath.Find(_T("\\..\\"))) != -1)
		{
			*((TCHAR*)(const TCHAR*)strPath + n) = 0;
			m = strPath.ReverseFind(_T('\\'));
			*((TCHAR*)(const TCHAR*)strPath + n) = _T('\\');
			if (m == -1)
				{ASSERT(false); break;} // cannot navigate to level above root!
			strPath = strPath.Left(m) + strPath.Mid(n+3);
		}
	}

	return strPath;
}

inline RString RelativePath(RString strPath)
{
	strPath = CorrectPath(strPath, true);
	RString strAppDir = GetAppDir();
	if (_tcsnicmp(CorrectPath(strPath, true), strAppDir, 
			min(strPath.GetLength(), strAppDir.GetLength())) == 0)
	{
		strPath = strPath.Mid(strAppDir.GetLength());
		strPath.TrimLeft(_T(" \\/"));
	}
	return strPath;
}

inline RString GetDir(RString_ strFilePath)
{
	RString strDir;
	INT_PTR n = strFilePath.ReverseFind(_T('\\'));
	if (n == -1)
		{ASSERT(false); return strDir;}
	strDir = strFilePath.Left(n);
	return strDir;
}


inline LONG DeleteDirectoryAndAllSubfolders(const TCHAR* lpszDirectory)
{
	TCHAR szDir[MAX_PATH + 1];  // +1 for the double null terminate
	SHFILEOPSTRUCTW fos = { 0 };

	StringCchCopy(szDir, MAX_PATH, lpszDirectory);
	int len = lstrlenW(szDir);
	szDir[len + 1] = 0; // double null terminate for SHFileOperation

	// delete the folder and everything inside
	fos.wFunc = FO_DELETE;
	fos.pFrom = szDir;
	fos.fFlags = FOF_NO_UI;
	return SHFileOperation(&fos);
}


inline RString GetComputerName()
{
	static RString str;
	if (str.IsEmpty())
	{
		bool bSuccess = false;
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters"), 
				0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwType;
			DWORD dwLen = 100;
			bSuccess = (RegQueryValueEx(hKey, _T("Hostname"), NULL, &dwType, 
					(BYTE*)str.GetBuffer(dwLen), &dwLen) == ERROR_SUCCESS);
			str.ReleaseBuffer();
			RegCloseKey(hKey);
		}

		if (!bSuccess)
		{
			DWORD dwLen = 100;
			GetComputerName(str.GetBuffer(dwLen + 1), &dwLen);
			str.ReleaseBuffer(dwLen);
		}
	}
	return str;
}

inline bool FileExists(const TCHAR* lpszFilePath)
{
	DWORD dwAttr = GetFileAttributes(lpszFilePath);
	return ((dwAttr != 0xFFFFFFFF) && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

inline bool DirectoryExists(const TCHAR* lpszDirPath)
{
	DWORD dwAttr = GetFileAttributes(lpszDirPath);
	return ((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

inline RString FixLineEnds(RString str, const TCHAR *lpszDesired = _T("\r\n"))
{
	str.Replace(_T("\r\n"), _T("\x1B\x1B"));
	str.Replace(_T("\r"), _T("\x1B\x1B"));
	str.Replace(_T("\n"), _T("\x1B\x1B"));
	str.Replace(_T("\x1B\x1B"), lpszDesired);
	return str;
}

#ifdef __RFONT_H__
inline HFONT GetDialogFont()
{
	static RFont font;
	static float fScale = GetScale();
	if (!font || fScale != GetScale())
	{
		NONCLIENTMETRICS ncm;
		ncm.cbSize= sizeof(NONCLIENTMETRICS);
		if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
			{ASSERT(false); return NULL;}
		ncm.lfMessageFont.lfHeight = (int)(ncm.lfMessageFont.lfHeight * GetScale() / GetSystemScale());
		VERIFY(font.CreateIndirect(&ncm.lfMessageFont));
		fScale = GetScale();
	}
	return font;
}
#endif // __RFONT_H__

inline INT_PTR StringToNumber(const TCHAR *lpsz)
{
	INT_PTR res = 0;
#if defined(_WIN64)
	if (lpsz)
		res = _ttoi64(lpsz);
#else
	if (lpsz)
		res = _ttoi(lpsz);
#endif
	return res;
}

inline INT64 StringToNumber64(const TCHAR *lpsz)
{
	INT64 res = 0;
	if (lpsz)
		res = _ttoi64(lpsz);
	return res;
}

inline RString NumberToString(int n, int radix = 10)
{
	RString str;
	_itot(n, str.GetBuffer(32), radix);
	str.ReleaseBuffer();
	return str;
}

inline RString NumberToString(INT64 n, int radix = 10)
{
	RString str;
	_i64tot(n, str.GetBuffer(64), radix);
	str.ReleaseBuffer();
	return str;
}

inline _locale_t GetLocaleC()
{
	// TODO: should call _free_locale at program termination
	static _locale_t localeC = _create_locale(LC_ALL, "C");
	return localeC;
}

inline RString FloatToString(float f)
{
	RString str;
	_stprintf_l(str.GetBuffer(32), _T("%g"), GetLocaleC(), f);
	str.ReleaseBuffer();
	return str;
}

inline RString DoubleToString(double d)
{
	RString str;
	_stprintf_l(str.GetBuffer(32), _T("%g"), GetLocaleC(), d);
	str.ReleaseBuffer();
	return str;
}

inline float StringToFloat(const TCHAR *lpsz)
{
	float res;
	if (!lpsz || _stscanf_l(lpsz, _T("%g"), GetLocaleC(), &res) != 1)
		res = 0.0f;
	return res;
}

inline double StringToDouble(const TCHAR *lpsz)
{
	double res;
	if (!lpsz || _stscanf_l(lpsz, _T("%lg"), GetLocaleC(), &res) != 1)
		res = 0.0;
	return res;
}

inline RString SizeToString(INT64 nSize)
{
	RString strSize;

	if (nSize >= 1024)
	{
		double dSize = (double)nSize;
		RString str = _T("bytes");

		dSize /= 1024.0;
		str = _T("KB");

		if (dSize >= 1024.0)
		{
			dSize /= 1024.0;
			str = _T("MB");

			if (dSize >= 1024.0)
			{
				dSize /= 1024.0;
				str = _T("GB");

				if (dSize >= 1024.0)
				{
					dSize /= 1024.0;
					str = _T("TB");
				}
			}
		}

		_stprintf_l(strSize.GetBuffer(32), _T("%.2f ") + str, GetLocaleC(), dSize);
		strSize.ReleaseBuffer();

		RString strDecimalSymbol;
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, strDecimalSymbol.GetBuffer(3), 4);
		strDecimalSymbol.ReleaseBuffer();
		strSize.Replace(_T("."), strDecimalSymbol);
	}
	else
	{
		_stprintf(strSize.GetBuffer(32), _T("%d bytes"), (int)nSize);
		strSize.ReleaseBuffer();
	}

	return strSize;
}

inline COLORREF StringToColor(RString str)
{
	// Parse string to DWORD value

	str.MakeLower();
	str.Replace(_T("#"), _T(""));

	TCHAR c;
	DWORD dw = 0;
	INT_PTR nLen = str.GetLength();

	if (nLen > 8)
		return 0;//ASSERTRETURN(0);

	for (INT_PTR i = 0; i < nLen; i++)
	{
		c = str[i];

		if (c >= '0' && c <= '9')
			dw |= (c - '0') << ((nLen - i - 1) * 4);
		else if (c >= 'a' && c <= 'f')
			dw |= (c - 'a' + 10) << ((nLen - i - 1) * 4);
		else
			return 0;//ASSERTRETURN(0);
	}

	// Convert DWORD value to color

	return RGB(GetBValue(dw), GetGValue(dw), GetRValue(dw));
}

inline RString ColorToString(COLORREF clr)
{
	RString str;
	_stprintf(str.GetBuffer(16), _T("#%.6x"), RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)));
	str.ReleaseBuffer();
	return str;
}

inline RString GetDecimalSep()
{
	static RString str;
	if (str.IsEmpty())
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, str.GetBuffer(3), 4);
		str.ReleaseBuffer();
	}
	return str;
}

inline RString GetThousandSep()
{
	static RString str;
	if (str.IsEmpty())
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, str.GetBuffer(3), 4);
		str.ReleaseBuffer();
	}
	return str;
}

inline void PostChildren(HWND hParentWnd, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0)
{
	HWND hWndChild = ::GetWindow(hParentWnd, GW_CHILD);
	while (hWndChild)
	{
		PostMessage(hWndChild, Msg, wParam, lParam);
		hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
	}
}

inline void PostChildrenRec(HWND hParentWnd, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0)
{
	HWND hWndChild = ::GetWindow(hParentWnd, GW_CHILD);
	while (hWndChild)
	{
		PostMessage(hWndChild, Msg, wParam, lParam);
		PostChildrenRec(hWndChild, Msg, wParam, lParam);
		hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
	}
}

inline void PostParents(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent = GetParent(hWnd);
	while (hWndParent)
	{
		PostMessage(hWndParent, Msg, wParam, lParam);
		hWndParent = GetParent(hWndParent);
	}
}

inline void SendChildren(HWND hParentWnd, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0)
{
	HWND hWndChild = ::GetWindow(hParentWnd, GW_CHILD);
	while (hWndChild)
	{
		SendMessage(hWndChild, Msg, wParam, lParam);
		hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
	}
}

inline void SendChildrenRec(HWND hParentWnd, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0)
{
	HWND hWndChild = ::GetWindow(hParentWnd, GW_CHILD);
	while (hWndChild)
	{
		SendMessage(hWndChild, Msg, wParam, lParam);
		SendChildrenRec(hWndChild, Msg, wParam, lParam);
		hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
	}
}

inline void SendParents(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent = GetParent(hWnd);
	while (hWndParent)
	{
		SendMessage(hWndParent, Msg, wParam, lParam);
		hWndParent = GetParent(hWndParent);
	}
}

inline HWND GetTopLevelParent(HWND hWnd)
{
	ASSERT(IsWindow(hWnd));

	HWND hWndTmp;
	while (hWndTmp = GetParent(hWnd))
		hWnd = hWndTmp;

	return hWnd;
}

#if defined(__RMEMORYDC_H__) && defined(__RFONT_H__)
inline int DialogUnitsToPixels(int n, bool bX)
{
	static int baseUnitX = 0;
	static int baseUnitY = 0;

	static float fScale = 0.0f;
	if (fScale != GetScale())
	{
		RMemoryDC mdc(0, 0);
		HFONT hOldFont = (HFONT)SelectObject(mdc, GetDialogFont());
		SIZE sz;
		GetTextExtentPoint32(mdc, _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"), 52, &sz);
		baseUnitX = (sz.cx / 26 + 1) / 2;
		baseUnitY = sz.cy;
		if (baseUnitY < 14)
			baseUnitY = 14;
		SelectObject(mdc, hOldFont);

		fScale = GetScale();
	}

	if (bX)
		return (n * baseUnitX) / 4;

	return (n * baseUnitY) / 8;
}
#endif

inline bool GetWindowRectRelative(HWND hWnd, HWND hWndRelativeTo, RECT *pRect)
{
	RECT rcWnd, rcRelativeTo;
	if (!GetWindowRect(hWnd, &rcWnd) || !GetWindowRect(hWndRelativeTo, &rcRelativeTo))
		return false;

	pRect->top = rcWnd.top - rcRelativeTo.top;
	pRect->left = rcWnd.left - rcRelativeTo.left;
	pRect->right = rcWnd.right - rcRelativeTo.left;
	pRect->bottom = rcWnd.bottom - rcRelativeTo.top;

	return true;
}

inline bool GetClientRectRelative(HWND hWnd, HWND hWndRelativeTo, RECT *pRect)
{
	RECT rcWnd, rcRelativeTo;
	if (!GetClientRect(hWnd, &rcWnd) ||
			!ClientToScreen(hWnd, (POINT*)&rcWnd) ||
			!ClientToScreen(hWnd, (POINT*)&rcWnd + 1) ||
			!GetClientRect(hWndRelativeTo, &rcRelativeTo) || 
			!ClientToScreen(hWndRelativeTo, (POINT*)&rcRelativeTo) ||
			!ClientToScreen(hWndRelativeTo, (POINT*)&rcRelativeTo + 1))
		return false;

	pRect->top = rcWnd.top - rcRelativeTo.top;
	pRect->left = rcWnd.left - rcRelativeTo.left;
	pRect->right = rcWnd.right - rcRelativeTo.left;
	pRect->bottom = rcWnd.bottom - rcRelativeTo.top;

	return true;
}

// TODO: This function interferes with the other one, compiler can't decide which function to 
// choose. How to fix?
//
//inline RObArray<RString> SplitString(const TCHAR *lpszTarget, INT_PTR nTargetLen, 
//		const TCHAR *lpszSeparator, INT_PTR nSeperatorLen, bool bSkipEmpty = false)
//{
//	ASSERT(lpszTarget);
//	ASSERT(nTargetLen >= 0);
//	ASSERT(lpszSeparator);
//	ASSERT(nSeperatorLen > 0);
//
//	RObArray<RString> parts;
//	const TCHAR *p1 = lpszTarget, *p2, *p3 = lpszTarget + nTargetLen;
//	RString *pStr;
//	INT_PTR nLen;
//	
//	while (true)
//	{
//		p2 = _tcsstr(p1, lpszSeparator);
//		if (!p2)
//			p2 = p3;
//		
//		if (!bSkipEmpty || p1 != p2)
//		{
//			nLen = p2 - p1;
//			pStr = parts.AddNew();
//			if (p1 != p2)
//			{
//				_tcsncpy(pStr->GetBuffer(nLen), p1, nLen);
//				pStr->ReleaseBuffer(nLen);
//			}
//		}
//
//		if (p2 == p3)
//			break;
//
//		p1 = p2 + nSeperatorLen;
//	}
//
//	return parts;
//}
//
//inline RObArray<RString> SplitString(const RString& strTarget, 
//		const RString& strSeperator, bool bSkipEmpty = false)
//	{return SplitString(strTarget, strTarget.GetLength(), strSeperator, 
//			strSeperator.GetLength(), bSkipEmpty);}

inline RArray<const TCHAR*> SplitString(TCHAR *lpszTarget, INT_PTR nTargetLen, 
		const TCHAR *lpszSeparator, INT_PTR nSeperatorLen, bool bSkipEmpty = false)
{
	// Note #1: This function modifies the target string (for efficiency)!
	// Note #2: The target and separator string need to have a zero at the specified length 
	//          position, otherwise the search will just continue until it reaches the 
	//          trailing zero. And separator is always used until the trailing zero.

	ASSERT(lpszTarget);
	ASSERT(nTargetLen >= 0);
	ASSERT(lpszSeparator);
	ASSERT(nSeperatorLen > 0);

	RArray<const TCHAR*> parts;
	TCHAR *p1 = lpszTarget, *p2, *p3 = lpszTarget + nTargetLen;

	while (true)
	{
		p2 = _tcsstr(p1, lpszSeparator);
		if (!p2)
			p2 = p3;
		else
			*p2 = 0;

		if (!bSkipEmpty || p1 != p2)
			parts.Add(p1);

		if (p2 == p3)
			break;
		p1 = p2 + nSeperatorLen;
	}

	return parts;
}

inline RArray<const TCHAR*> SplitString(RString &strTarget, const RString& strSeparator,
			bool bSkipEmpty = false)
	{return SplitString(strTarget.GetBuffer(strTarget.GetLength()), strTarget.GetLength(), 
			strSeparator, strSeparator.GetLength(), bSkipEmpty);}

inline HWND GetConsoleHwnd()
{
    HWND hwndFound;
    TCHAR newWindowTitle[1024];
    TCHAR oldWindowTitle[1024];
    GetConsoleTitle(oldWindowTitle, 1024);
    wsprintf(newWindowTitle, _T("%d/%d"), GetTickCount(), GetCurrentProcessId());
    SetConsoleTitle(newWindowTitle);
    Sleep(10);
    hwndFound = FindWindow(NULL, newWindowTitle);
    SetConsoleTitle(oldWindowTitle);
    return hwndFound;
}

struct FILEINFO
{
	RString strName, strDosName;
	DWORD attributes;
	UINT64 size, creationTime, lastAccessTime, lastWriteTime;
	bool bDirectory;
};

inline RObArray<FILEINFO> EnumFiles(RString_ strDirPath, RString_ strMask = _T("*"), 
		bool bIncludeDirs = false)
{
	RObArray<FILEINFO> fileInfos;
	WIN32_FIND_DATA ffd;
	FILEINFO *pFI;

#ifdef _UNICODE
	HANDLE hFindFile = FindFirstFile(_T("\\\\?\\") + strDirPath + _T("\\") + strMask, &ffd);
#else
	HANDLE hFindFile = FindFirstFile(strDirPath + _T("\\") + strMask, &ffd);
#endif
	
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcscmp(ffd.cFileName, _T(".")) == 0 || _tcscmp(ffd.cFileName, _T("..")) == 0)
				continue;

			if (!bIncludeDirs && (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;

			pFI = fileInfos.AddNew();
			pFI->strName = ffd.cFileName;
			pFI->strDosName = ffd.cAlternateFileName;
			pFI->attributes = ffd.dwFileAttributes;
			pFI->bDirectory = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			*((DWORD*)&pFI->size) = ffd.nFileSizeLow;
			*(((DWORD*)&pFI->size) + 1) = ffd.nFileSizeHigh;
			*((DWORD*)&pFI->creationTime) = ffd.ftCreationTime.dwLowDateTime;
			*(((DWORD*)&pFI->creationTime) + 1) = ffd.ftCreationTime.dwHighDateTime;
			*((DWORD*)&pFI->lastAccessTime) = ffd.ftLastAccessTime.dwLowDateTime;
			*(((DWORD*)&pFI->lastAccessTime) + 1) = ffd.ftLastAccessTime.dwHighDateTime;
			*((DWORD*)&pFI->lastWriteTime) = ffd.ftLastWriteTime.dwLowDateTime;
			*(((DWORD*)&pFI->lastWriteTime) + 1) = ffd.ftLastWriteTime.dwHighDateTime;

		} while (FindNextFile(hFindFile, &ffd));
		
		FindClose(hFindFile);
	}

	return fileInfos;
}

inline RObArray<FILEINFO> EnumDirs(RString_ strDirPath, RString_ strMask = _T("*"))
{
	RObArray<FILEINFO> fileInfos;
	WIN32_FIND_DATA ffd;
	FILEINFO *pFI;

#ifdef _UNICODE
	HANDLE hFindFile = FindFirstFile(_T("\\\\?\\") + strDirPath + _T("\\") + strMask, &ffd);
#else
	HANDLE hFindFile = FindFirstFile(strDirPath + _T("\\") + strMask, &ffd);
#endif
	
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;

			if (_tcscmp(ffd.cFileName, _T(".")) == 0 || _tcscmp(ffd.cFileName, _T("..")) == 0)
				continue;

			pFI = fileInfos.AddNew();
			pFI->strName = ffd.cFileName;
			pFI->strDosName = ffd.cAlternateFileName;
			pFI->attributes = ffd.dwFileAttributes;
			pFI->bDirectory = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			*((DWORD*)&pFI->size) = ffd.nFileSizeLow;
			*(((DWORD*)&pFI->size) + 1) = ffd.nFileSizeHigh;
			*((DWORD*)&pFI->creationTime) = ffd.ftCreationTime.dwLowDateTime;
			*(((DWORD*)&pFI->creationTime) + 1) = ffd.ftCreationTime.dwHighDateTime;
			*((DWORD*)&pFI->lastAccessTime) = ffd.ftLastAccessTime.dwLowDateTime;
			*(((DWORD*)&pFI->lastAccessTime) + 1) = ffd.ftLastAccessTime.dwHighDateTime;
			*((DWORD*)&pFI->lastWriteTime) = ffd.ftLastWriteTime.dwLowDateTime;
			*(((DWORD*)&pFI->lastWriteTime) + 1) = ffd.ftLastWriteTime.dwHighDateTime;

		} while (FindNextFile(hFindFile, &ffd));
		
		FindClose(hFindFile);
	}

	return fileInfos;
}

inline RObArray<RString> EnumFileNames(RString_ strDir, RString_ strMask = _T("*"))
{
	RObArray<RString> fileNames;
	WIN32_FIND_DATA ffd;

#ifdef _UNICODE
	HANDLE hFindFile = FindFirstFile(_T("\\\\?\\") + strDir + _T("\\") + strMask, &ffd);
#else
	HANDLE hFindFile = FindFirstFile(strDir + _T("\\") + strMask, &ffd);
#endif

	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			fileNames.Add(ffd.cFileName);

		} while (FindNextFile(hFindFile, &ffd));
		
		FindClose(hFindFile);
	}

	return fileNames;
}

inline RObArray<RString> EnumDirNames(RString_ strDir)
{
	RObArray<RString> fileNames;
	WIN32_FIND_DATA ffd;

#ifdef _UNICODE
	HANDLE hFindFile = FindFirstFile(_T("\\\\?\\") + strDir + _T("\\*"), &ffd);
#else
	HANDLE hFindFile = FindFirstFile(strDir + _T("\\*"), &ffd);
#endif

	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				fileNames.Add(ffd.cFileName);

		} while (FindNextFile(hFindFile, &ffd));
		
		FindClose(hFindFile);
	}

	return fileNames;
}

inline RString GetFileName(RString strFilePath)
{
	strFilePath.Replace(_T('/'), _T('\\'));
	INT_PTR n = strFilePath.ReverseFind(_T('\\'));
	if (n != -1)
		strFilePath = strFilePath.Mid(n+1);
	return strFilePath;
}

inline RString GetFileExt(RString_ strFilePath)
{
	// NOTE: It could also be a directory with a dot in its name.
	RString strExt;
	RString strFileName = GetFileName(strFilePath);
	INT_PTR n = strFileName.ReverseFind(_T('.'));
	if (n != -1)
		strExt = strFileName.Mid(n+1);
	return strExt;
}

inline bool FillSolidRect(HDC hDC, const RECT *lprc, COLORREF clr)
{
	COLORREF clrPrev = SetBkColor(hDC, clr);
	bool ret = ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL) != FALSE;
	SetBkColor(hDC, clrPrev);
	return ret;
}

inline bool FillSolidRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clr)
{
	RECT rc = {x, y, x + cx, y + cy};
	return FillSolidRect(hDC, &rc, clr);
}

inline bool IsWin8()
{
	OSVERSIONINFO vi;
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	VERIFY(GetVersionEx(&vi));
	return (vi.dwMajorVersion == 6 && vi.dwMinorVersion == 2);
}

inline bool IsWin7()
{
	OSVERSIONINFO vi;
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	VERIFY(GetVersionEx(&vi));
	return (vi.dwMajorVersion == 6 && vi.dwMinorVersion == 1);
}

inline bool IsWinVista()
{
	OSVERSIONINFO vi;
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	VERIFY(GetVersionEx(&vi));
	return (vi.dwMajorVersion == 6 && vi.dwMinorVersion == 0);
}

inline bool IsWinXP()
{
	OSVERSIONINFO vi;
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	VERIFY(GetVersionEx(&vi));
	return (vi.dwMajorVersion == 5 && vi.dwMinorVersion == 1);
}

inline bool IsWin2K()
{
	OSVERSIONINFO vi;
	ZeroMemory(&vi, sizeof(OSVERSIONINFO));
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	VERIFY(GetVersionEx(&vi));
	return (vi.dwMajorVersion == 5 && vi.dwMinorVersion == 0);
}

inline RString_ GetEmptyString()
{
	static RString strEmpty;
	return strEmpty;
}

inline RString_ GetEmptyStr()
	{return GetEmptyString();}

inline BOOL ClientToScreen(HWND hWnd, RECT *pRect)
	{return (ClientToScreen(hWnd, (POINT*)pRect) && ClientToScreen(hWnd, (POINT*)pRect + 1));}

inline BOOL ScreenToClient(HWND hWnd, RECT *pRect)
	{return (ScreenToClient(hWnd, (POINT*)pRect) && ScreenToClient(hWnd, (POINT*)pRect + 1));}

inline BOOL PtInRect(const RECT *pRect, int x, int y)
	{return (x >= pRect->left && x < pRect->right && y >= pRect->top && y < pRect->bottom);}

inline BOOL TextOut(HDC hDC, int x, int y, RString_ str)
	{return TextOut(hDC, x, y, str, (int)str.GetLength());}

inline BOOL DrawText(HDC hDC, RString_ str, RECT *pRect, UINT format)
	{return DrawText(hDC, str, (int)str.GetLength(), pRect, format);}

inline BOOL GetTextExtentPoint32(HDC hDC, RString_ str, SIZE *pSize)
	{return GetTextExtentPoint32(hDC, str, (int)str.GetLength(), pSize);}

inline BOOL PostMessage(HWND hWnd, UINT Msg, WPARAM wParam)
	{return PostMessage(hWnd, Msg, wParam, 0);}

inline BOOL PostMessage(HWND hWnd, UINT Msg)
	{return PostMessage(hWnd, Msg, 0, 0);}

inline LRESULT SendMessage(HWND hWnd, UINT Msg, WPARAM wParam)
	{return SendMessage(hWnd, Msg, wParam, 0);}

inline LRESULT SendMessage(HWND hWnd, UINT Msg)
	{return SendMessage(hWnd, Msg, 0, 0);}

inline BOOL PostThreadMessage(DWORD idThread, UINT Msg, WPARAM wParam)
	{return PostThreadMessage(idThread, Msg, wParam, 0);}

inline BOOL PostThreadMessage(DWORD idThread, UINT Msg)
	{return PostThreadMessage(idThread, Msg, 0, 0);}

inline LRESULT CallWindowProc(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam)
	{return CallWindowProc(lpPrevWndFunc, hWnd, Msg, wParam, 0);}

inline LRESULT CallWindowProc(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg)
	{return CallWindowProc(lpPrevWndFunc, hWnd, Msg, 0, 0);}

inline BOOL MoveWindow(HWND hWnd, const RECT *pRect, BOOL bRepaint)
	{return MoveWindow(hWnd, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, bRepaint);}

inline BOOL MoveWindow(HWND hWnd, const RECT *pRect)
	{return MoveWindow(hWnd, pRect, TRUE);}

inline BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight)
	{return MoveWindow(hWnd, X, Y, nWidth, nHeight, TRUE);}

inline BOOL Invalidate(HWND hWnd)
	{return InvalidateRect(hWnd, NULL, FALSE);}

inline BOOL CreateDirectory(const TCHAR *lpszPathName)
	{return CreateDirectory(lpszPathName, NULL);}

inline BOOL BitBlt(HDC hDC, int x, int y, int cx, int cy, HDC hSrcDC, int xSrc = 0, int ySrc = 0)
	{return BitBlt(hDC, x, y, cx, cy, hSrcDC, xSrc, ySrc, SRCCOPY);}

inline BOOL BitBlt(HDC hDC, const RECT *pRect, HDC hSrcDC, int xSrc = 0, int ySrc = 0)
	{return BitBlt(hDC, pRect->left, pRect->top, pRect->right - pRect->left, 
			pRect->bottom - pRect->top, hSrcDC, xSrc, ySrc, SRCCOPY);}

inline INT64 QueryPerformanceFrequency()
{
	LARGE_INTEGER li = {0};
	QueryPerformanceFrequency(&li);
	return li.QuadPart;
}

inline INT64 QueryPerformanceCounter()
{
	LARGE_INTEGER li = {0};
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}
