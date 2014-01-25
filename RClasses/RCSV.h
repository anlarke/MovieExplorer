#pragma once

// TODO:
// - Use combination of RObArrayNRC and RArrayNRC
// - Use const TCHAR* instead of LPCTSTR


//
// RCSVFile
//

class RCSVFile
{
public:
	RCSVFile()
	{
	}

	virtual ~RCSVFile()
	{
	}

protected:

};

//
// RCSVFileRO (for read-only operations on CSV files)
//

class RCSVFileRO
{
public:
	RCSVFileRO() : m_nFieldCount(0)
	{
	}

	virtual ~RCSVFileRO()
	{
		Close();
	}

	bool Open(LPCTSTR lpszFile)
	{
		Close();

		// store file in buffer, taking into account file encoding

		FileToString(lpszFile, m_strBuffer);

		/*
		HANDLE hFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		DWORD dwSize = GetFileSize(hFile, NULL);
		BYTE* pReadBuf = new BYTE[dwSize + 2]; // two trailing zero bytes
		memset(pReadBuf, 0, dwSize + 2);
		DWORD dwRead;
		ReadFile(hFile, pReadBuf, dwSize, &dwRead, NULL);
		CloseHandle(hFile);

		if (pReadBuf[0] == 0xEF && pReadBuf[1] == 0xBB && pReadBuf[2] == 0xBF)
		{
			DWORD cch_plusone = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(pReadBuf + 3), -1, NULL, 0);
			WCHAR* pWideStr = new WCHAR[cch_plusone];
			MultiByteToWideChar(CP_UTF8, 0, (const char*)(pReadBuf + 3), -1, pWideStr, cch_plusone);
			m_strBuffer = pWideStr;
			delete[] pWideStr;
		}
		else if (pReadBuf[0] == 0xFF && pReadBuf[1] == 0xFE)
			m_strBuffer = (const WCHAR*)(pReadBuf + 2);
		else
			m_strBuffer = (const char*)pReadBuf;

		delete[] pReadBuf;
		*/

		/*
		// determine line endings

		RString strLineEnd;
		if (m_strBuffer.Find(_T("\r\n")))
			strLineEnd = _T("\r\n");
		else if (m_strBuffer.Find(_T('\r')))
			strLineEnd = _T("\r");
		else if (m_strBuffer.Find(_T('\n')))
			strLineEnd = _T("\n");
		else
			strLineEnd = _T("\r\n");
		*/

		/*
		// parse contents (support line endings in fields)

		LPCTSTR lpszBufferEnd = (LPCTSTR)m_strBuffer + m_strBuffer.GetLength();
		LPCTSTR lpszStart, lpszEnd, lpszStart2, lpszEnd2;
		RArray<LPCTSTR> *pLine;
		
		pLine = new RArray<LPCTSTR>;
		m_lines.Add(pLine);

		lpszStart = m_strBuffer;
		while (true)
		{
			if (*lpszStart == _T('"')) // field enclosed by quotes
			{
				lpszStart++;
				lpszStart2 = lpszStart;
				while (true)
				{
					lpszEnd = _tcschr(lpszStart2, _T('"'));
					if (!lpszEnd)
						{Close(); return false;} // fields enclosed by quotes should always be closed
					if (lpszEnd+1 == lpszBufferEnd || *(lpszEnd+1) != _T('"'))
						break;
					lpszStart2 = lpszEnd+2; // skip next quote in pair
				}

				*(LPTSTR)lpszEnd = 0;
				pLine->Add(lpszStart);

				if (_tcschr(lpszStart, _T('"')))
				{
					RString strTemp(lpszStart);
					strTemp.Replace(_T("\"\""), _T("\""));
					_tcscpy((LPTSTR)lpszStart, strTemp);
				}

				if (lpszEnd+1 == lpszBufferEnd) // field ends at end of buffer
				{
					m_nFieldCount = max(m_nFieldCount, pLine->GetSize());
					break;
				}
				else if (*(lpszEnd+1) == _T(';')) // field ends at delimiter
				{
					lpszStart = lpszEnd + 2;
				}
				else // field ends at line end
				{
					m_nFieldCount = max(m_nFieldCount, pLine->GetSize());
					lpszStart = lpszEnd + 3;
					pLine = new RArray<LPCTSTR>;
					m_lines.Add(pLine);
				}
			}
			else // normal field
			{
				lpszEnd = _tcschr(lpszStart, _T(';'));
				lpszEnd2 = _tcsstr(lpszStart, _T("\r\n"));

				if (!lpszEnd && !lpszEnd2) // field ends at end of buffer
				{
					lpszEnd = (LPCTSTR)m_strBuffer + m_strBuffer.GetLength();
					pLine->Add(lpszStart);
					m_nFieldCount = max(m_nFieldCount, pLine->GetSize());
					break;
				}
				else if (!lpszEnd && lpszEnd2) // field ends at line end
				{
					lpszEnd = lpszEnd2;
					*(LPTSTR)lpszEnd = 0;
					pLine->Add(lpszStart);
					m_nFieldCount = max(m_nFieldCount, pLine->GetSize());
					lpszStart = lpszEnd + 2;
					pLine = new RArray<LPCTSTR>;
					m_lines.Add(pLine);
				}
				else if (lpszEnd && !lpszEnd2) // field ends at delimiter
				{
					*(LPTSTR)lpszEnd = 0;
					pLine->Add(lpszStart);
					lpszStart = lpszEnd + 1;
				}
				else //if (lpszEnd && lpszEnd2)
				{
					if (lpszEnd2 < lpszEnd) // field ends at line end
					{
						lpszEnd = lpszEnd2;
						*(LPTSTR)lpszEnd = 0;
						pLine->Add(lpszStart);
						m_nFieldCount = max(m_nFieldCount, pLine->GetSize());
						lpszStart = lpszEnd + 2;
						pLine = new RArray<LPCTSTR>;
						m_lines.Add(pLine);
					}
					else // field ends at delimiter
					{
						*(LPTSTR)lpszEnd = 0;
						pLine->Add(lpszStart);
						lpszStart = lpszEnd + 1;
					}
				}
			}
		}
		*/

		// parse contents (do NOT support line endings in fields)

		LPCTSTR lpszBufferStart = m_strBuffer;
		LPCTSTR lpszBufferEnd = lpszBufferStart + m_strBuffer.GetLength();

		LPCTSTR lpszStart, lpszEnd, lpszStart2, lpszEnd2, lpszStart3;
		RArray<LPCTSTR> *pLine;

		lpszStart = lpszBufferStart;
		while (true)
		{
			lpszEnd = _tcsstr(lpszStart, _T("\r\n"));
			if (!lpszEnd)
				lpszEnd = lpszBufferEnd;
			else
				*(LPTSTR)lpszEnd = 0;

			pLine = new RArray<LPCTSTR>;
			m_lines.Add(pLine);
			
			lpszStart2 = lpszStart;
			while (true)
			{
				if (*lpszStart2 == _T('"')) // field enclosed by quotes
				{
					lpszStart2++;
					lpszStart3 = lpszStart2;
					while (true)
					{
						lpszEnd2 = _tcschr(lpszStart3, _T('"'));
						if (!lpszEnd2)
							{Close(); return false;} // fields enclosed by quotes should always be closed
						if (lpszEnd2+1 == lpszBufferEnd || *(lpszEnd2+1) != _T('"'))
							break;
						lpszStart3 = lpszEnd2+2; // skip next quote in pair
					}
					*(LPTSTR)lpszEnd2 = 0;

					if (_tcschr(lpszStart2, _T('"')))
					{
						RString strTemp(lpszStart2);
						strTemp.Replace(_T("\"\""), _T("\""));
						_tcscpy((LPTSTR)lpszStart2, strTemp);
					}

					pLine->Add(lpszStart2);
					m_nFieldCount = max(m_nFieldCount, pLine->GetSize());

					if (lpszEnd2+1 == lpszEnd)
						break;
					lpszStart2 = lpszEnd2 + 2;
				}
				else // normal field
				{
					lpszEnd2 = _tcschr(lpszStart2, _T(';'));
					if (!lpszEnd2)
						lpszEnd2 = lpszEnd;
					else
						*(LPTSTR)lpszEnd2 = 0;

					pLine->Add(lpszStart2);
					m_nFieldCount = max(m_nFieldCount, pLine->GetSize());

					if (lpszEnd2 == lpszEnd)
						break;
					lpszStart2 = lpszEnd2 + 1;
				}
			}

			if (lpszEnd == lpszBufferEnd)
				break;
			lpszStart = lpszEnd + 2;
		}

		return true;
	}

	void Close()
	{
		for (INT_PTR i = 0; i < m_lines; ++i)
			delete m_lines[i];
		m_lines.SetSize(0);
		m_strBuffer.Empty();
		m_nFieldCount = 0;
	}

	INT_PTR GetLineCount()
	{
		return m_lines;
	}
	
	INT_PTR GetFieldCount()
	{
		return m_nFieldCount;
	}

	LPCTSTR GetField(int nLine, int nField)
	{
		ASSERT(nLine < m_lines);
		ASSERT(m_nFieldCount > 0);
		if (nField >= *m_lines[nLine])
			return _T("");
		return (*m_lines[nLine])[nField];
	}

protected:
	INT_PTR m_nFieldCount;
	RString m_strBuffer;
	RArray<RArray<LPCTSTR>*> m_lines;
};
