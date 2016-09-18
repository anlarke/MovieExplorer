#pragma once

inline RString DataToString(const BYTE *pData, INT_PTR nDataLen, DWORD charSet = CHARSET_AUTODETECT)
{
	ASSERT(pData);
	ASSERT(nDataLen >= 0);
	ASSERT(sizeof(WCHAR) == 2);

	if (!pData || nDataLen == 0)
		return _T("");

	// Determine character set and offset

	INT_PTR nOffset;
	if (strncmp((LPCSTR)pData, "\xEF\xBB\xBF", 3) == 0)
	{
		nOffset = 3;
		if (charSet == CHARSET_AUTODETECT)
			charSet = CHARSET_UTF8;
	}
	else if (strncmp((LPCSTR)pData, "\xFF\xFE", 2) == 0)
	{
		nOffset = 2;
		if (charSet == CHARSET_AUTODETECT)
			charSet = CHARSET_UTF16LE;
	}
	else if (strncmp((LPCSTR)pData, "\xFE\xFF", 2) == 0)
	{
		nOffset = 2;
		if (charSet == CHARSET_AUTODETECT)
			charSet = CHARSET_UTF16BE;
	}
	else
	{
		nOffset = 0;
		if (charSet == CHARSET_AUTODETECT)
			charSet = CHARSET_ANSI;
	}

	// Copy data and flip bytes of big-endian UTF-16 string

	RArray<BYTE> copy;
	if (charSet == CHARSET_UTF16BE)
	{
		copy.SetSize(nDataLen);
		memcpy(copy.GetData(), pData, nDataLen);
		pData = copy.GetData();

		INT_PTR n = nDataLen / 2;
		WCHAR *p = (LPWSTR)pData;
		for (int i = 0; i < n; i++)
			*p++ = (*p >> 8) | (*p << 8);

		charSet = CHARSET_UTF16LE;
	}

	// Convert according to specified/detected character set

	RString str;
	if (charSet == CHARSET_UTF8)
	{
		#ifdef _UNICODE
		{
			int nLenWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(pData + nOffset), (int)(nDataLen - nOffset), NULL, 0);
			if (MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(pData + nOffset), (int)(nDataLen - nOffset), 
									str.GetBuffer(nLenWide), nLenWide) != nLenWide)
				{str.ReleaseBuffer(0); ASSERT(false); return str;}
			str.ReleaseBuffer(nLenWide);
		}
		#else
		{
			ASSERT(false); // you don't want to process unicode data in a non-unicode environment!
			int nLenWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(pData + nOffset), (int)(nDataLen - nOffset), NULL, 0);
			WCHAR *pStrWide = new WCHAR[nLenWide];
			if (MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(pData + nOffset), (int)(nDataLen - nOffset), pStrWide, nLenWide) != nLenWide)
				{delete pStrWide; ASSERT(false); return str;}
			int nLenMB = WideCharToMultiByte(CP_ACP, 0, pStrWide, nLenWide, NULL, 0, NULL, NULL);
			if (WideCharToMultiByte(CP_ACP, 0, pStrWide, nLenWide, str.GetBuffer(nLenMB), nLenMB, NULL, NULL) != nLenMB)
				{delete pStrWide; str.ReleaseBuffer(0); ASSERT(false); return str;}
			delete pStrWide;
			str.ReleaseBuffer(nLenMB);
		}
		#endif
	}
	else if (charSet == CHARSET_UTF16LE)
	{
		#ifdef _UNICODE
		{
			INT_PTR nLenWide = (nDataLen - nOffset) / 2;
			memcpy(str.GetBuffer(nLenWide), pData + nOffset, nDataLen - nOffset);
			str.ReleaseBuffer(nLenWide);
		}
		#else
		{
			ASSERT(false); // you don't want to process unicode data in a non-unicode environment!
			int nLenWide = (int)(nDataLen - nOffset) / 2;
			int nLenMB = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)(pData + nOffset), nLenWide, NULL, 0, NULL, NULL);
			if (WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)(pData + nOffset), nLenWide, 
									str.GetBuffer(nLenMB), nLenMB, NULL, NULL) != nLenMB)
				{str.ReleaseBuffer(0); ASSERT(false); return str;}
			str.ReleaseBuffer(nLenMB);
		}
		#endif
	}
	else if (charSet == CHARSET_ANSI)
	{
		#ifdef _UNICODE
		{
			WCHAR *p = str.GetBuffer(nDataLen);
			for (int i = 0; i < nDataLen; i++)
				*p++ = *(pData + i);
			str.ReleaseBuffer(nDataLen);
		}
		#else
		{
			memcpy(str.GetBuffer(nDataLen), pData, nDataLen);
			str.ReleaseBuffer(nDataLen);
		}
		#endif
	}
	else
		ASSERT(false); // character set not supported

	return str;
}

inline RString DataToString(const RArray<BYTE> &data, DWORD charSet = CHARSET_AUTODETECT)
	{return DataToString(data.GetData(), data.GetSize(), charSet);}

inline RArray<BYTE> StringToData(const TCHAR *lpsz, INT_PTR nLen, DWORD charSet, bool bIncludeBOM)
{
	ASSERT(lpsz);
	ASSERT(nLen >= 0);
	ASSERT(sizeof(WCHAR) == 2);

	// Convert according to specified character set

	RArray<BYTE> data;
	if (charSet == CHARSET_ANSI)
	{
		#ifdef _UNICODE
		{
			int nLenMB = WideCharToMultiByte(CP_ACP, 0, lpsz, (int)nLen, NULL, 0, NULL, NULL);
			data.SetSize(nLenMB);
			if (WideCharToMultiByte(CP_ACP, 0, lpsz, (int)nLen, (LPSTR)data.GetData(), nLenMB, NULL, NULL) != nLenMB)
				{data.SetSize(0); ASSERT(false);}
		}
		#else
		{
			// this actually assumes lpsz to be a SBCS (ANSI string)... 
			// maybe fix it by first calling MultiByteToWideChar and then WideCharToMultiByte?
			data.SetSize(nLen);
			memcpy(data.GetData(), lpsz, nLen);
		}
		#endif
	}
	else if (charSet == CHARSET_UTF16LE || charSet == CHARSET_UTF16BE)
	{
		int nOffset = 0;

		if (bIncludeBOM)
		{
			data.SetSize(2);
			memcpy(data.GetData(), "\xFF\xFE", 2);
			nOffset = 2;
		}

		#ifdef _UNICODE
		{
			data.SetSize(nOffset + nLen * sizeof(TCHAR));
			memcpy(data.GetData() + nOffset, lpsz, nLen * sizeof(TCHAR));
		}
		#else
		{
			int nLenWide = MultiByteToWideChar(CP_ACP, 0, lpsz, (int)nLen, NULL, 0);
			data.SetSize(nOffset + nLenWide * 2);
			if (MultiByteToWideChar(CP_ACP, 0, lpsz, (int)nLen, (LPWSTR)(data.GetData() + nOffset), nLenWide) != nLenWide)
				{data.SetSize(0); ASSERT(false);}
		}
		#endif
	}
	else if (charSet == CHARSET_UTF8)
	{
		int nOffset = 0;

		if (bIncludeBOM)
		{
			data.SetSize(3);
			memcpy(data.GetData(), "\xEF\xBB\xBF", 3);
			nOffset = 3;
		}

		#ifdef _UNICODE
		{
			int nLenMB = WideCharToMultiByte(CP_UTF8, 0, lpsz, (int)nLen, NULL, 0, NULL, NULL);
			data.SetSize(nOffset + nLenMB);
			if (WideCharToMultiByte(CP_UTF8, 0, lpsz, (int)nLen, (LPSTR)(data.GetData() + nOffset), nLenMB, NULL, NULL) != nLenMB)
				{data.SetSize(0); ASSERT(false);}
		}
		#else
		{
			// this also assumes lpsz to be a SBCS (ANSI string)...
			data.SetSize(nOffset + nLen);
			memcpy(data.GetData() + nOffset, lpsz, nLen);
		}
		#endif
	}
	else
		ASSERT(false); // character set not supported

	// Change BOM and flip bytes of big-endian UTF-16

	if (charSet == CHARSET_UTF16BE)
	{
		INT_PTR n = data.GetSize() / 2;
		WCHAR *p = (LPWSTR)data.GetData();
		for (INT_PTR i = 0; i < n; i++)
			*p++ = (*p >> 8) | (*p << 8);
	}

	return data;
}

inline RArray<BYTE> StringToData(RString_ str, DWORD charSet = CHARSET_UTF8, bool bIncludeBOM = false)
	{return StringToData(str, str.GetLength(), charSet, bIncludeBOM);}

inline bool DataToFile(const BYTE *pData, INT_PTR nDataLen, const TCHAR *lpszFilePath)
{
	RString strFilePath = CorrectPath(lpszFilePath);
	HANDLE hFile = CreateFile(strFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		{ASSERT(false); return false;} // failed to create file, does directory exist?
	DWORD dwWritten;
	if (!WriteFile(hFile, pData, (DWORD)nDataLen, &dwWritten, NULL) || dwWritten != nDataLen)
		{CloseHandle(hFile); ASSERT(false); return false;} // write error
	CloseHandle(hFile);
	return true;
}

inline bool DataToFile(const RArray<BYTE>& data, const TCHAR *lpszFilePath)
	{return DataToFile(data.GetData(), data.GetSize(), lpszFilePath);}

inline bool StringToFile(const TCHAR *lpszString, INT_PTR nStringLen, const TCHAR *lpszFilePath, DWORD charSet = CHARSET_UTF8, bool bIncludeBOM = true)
	{return DataToFile(StringToData(lpszString, nStringLen, charSet, bIncludeBOM), lpszFilePath);}

inline bool StringToFile(RString_ str, const TCHAR *lpszFilePath, DWORD charSet = CHARSET_UTF8, bool bIncludeBOM = true)
	{return DataToFile(StringToData(str, str.GetLength(), charSet, bIncludeBOM), lpszFilePath);}

inline bool FileToData(const TCHAR *lpszFilePath, RArray<BYTE>& data)
{
	RString strFilePath = CorrectPath(lpszFilePath);
	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwSizeLow, dwSizeHigh;
	dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
	if (dwSizeHigh > 0 || dwSizeLow > 0x7FFFFFFF)
		{CloseHandle(hFile); {ASSERT(false); return false;}} // this file is too large
	data.SetSize(dwSizeLow);
	DWORD dwRead;
	if (!ReadFile(hFile, data.GetData(), dwSizeLow, &dwRead, NULL) || dwRead != dwSizeLow)
		{CloseHandle(hFile); {ASSERT(false); return false;}} // read error
	CloseHandle(hFile);
	return true;
}

inline bool FileToString(const TCHAR *lpszFilePath, RString &str, DWORD charSet = CHARSET_AUTODETECT)
{
	RArray<BYTE> data;
	if (!FileToData(lpszFilePath, data))
		return false;
	str = DataToString(data, charSet);
	return true;
}

#ifdef _WINHTTPX_

inline bool URLToData(RString strURL, RArray<BYTE>& data, bool& bUTF8)
{
	data.SetSize(0);
	bUTF8 = false;

	// Https or Http

	if (strURL.GetLength() < 8)
		return false;

	bool bNormal = strURL.Left(7).Equals(_T("http://"));
	bool bSecure = strURL.Left(8).Equals(_T("https://"));
	if (!bSecure && !bNormal)
		return false;

	// Extract server and object name

	INTERNET_PORT nServerPort;
	DWORD dwFlags = 0;

	if (bNormal)
	{
		strURL.Replace(_T("http://"), _T(""));
		nServerPort = INTERNET_DEFAULT_HTTP_PORT;
	}
	else if (bSecure)
	{
		strURL.Replace(_T("https://"), _T(""));
		nServerPort = INTERNET_DEFAULT_HTTPS_PORT;
		dwFlags = WINHTTP_FLAG_SECURE;
	}

	if (strURL.Find(_T('/')) == -1)
		strURL += _T("/");

	RString strServerName = strURL.Left(strURL.Find(_T('/')));
	RString strObjectName = strURL.Mid(strURL.Find(_T('/')));

	// Open connection and send request

	HINTERNET hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession)
		return false;

	HINTERNET hConnect = WinHttpConnect(hSession, strServerName, nServerPort, 0);
	if (!hConnect)
		return false;

	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", strObjectName, NULL, 
			WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
	if (!hRequest)
		return false;

	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
			WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
		return false;

	if (!WinHttpReceiveResponse(hRequest, NULL))
		return false;

	// Check if the charset is UTF-8

	DWORD dwBufferLength = 256;
	RString strBuffer;
	if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_TYPE, WINHTTP_HEADER_NAME_BY_INDEX, 
			strBuffer.GetBuffer(dwBufferLength / 2), &dwBufferLength, WINHTTP_NO_HEADER_INDEX))
	{
		strBuffer.ReleaseBuffer(dwBufferLength / 2);
		if (strBuffer.FindNoCase(_T("UTF-8")) != -1)
			bUTF8 = true;
	}

	// Retrieve data

	RArray<BYTE> buffer;
	for (;;)
	{
		DWORD dwBytesAvailable = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &dwBytesAvailable))
			return false;
		if (dwBytesAvailable == 0)
			break;

		DWORD dwBytesRead = 0;
		buffer.SetSize(dwBytesAvailable);
		if (!WinHttpReadData(hRequest, buffer.GetData(), dwBytesAvailable, &dwBytesRead))
			return false;
		buffer.SetSize(dwBytesRead);

		data.Add(buffer);
	}

	// Clean up

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	return true;
}

inline bool URLToData(const TCHAR* lpszURL, RArray<BYTE>& data)
	{bool bUTF8; return URLToData(lpszURL, data, bUTF8);}

inline bool URLToString(const TCHAR* lpszURL, RString& str)
{
	str.Empty();

	RArray<BYTE> data;
	bool bUTF8;
	if (!URLToData(lpszURL, data, bUTF8))
		return false;

	str = DataToString(data, (bUTF8 ? CHARSET_UTF8 : CHARSET_ANSI));

	return true;
}

inline RString URLToString(const TCHAR* lpszURL)
	{RString str; URLToString(lpszURL, str); return str;}

#endif // _WINHTTPX_
