#ifndef __RXML_H__
#define __RXML_H__

// alles wat hier een "tag" is zou "element" moeten zijn
// alles wat hier een "property" is zou "attribute" moeten zijn

#define XMLFILE_CHARACTERSET_ANSI		1
#define XMLFILE_CHARACTERSET_UNICODE	2
#define XMLFILE_CHARACTERSET_UTF8		3

//
//
// RXMLTag class
//
//

class RXMLTag
{
	friend class RXMLFile;
public:
	RXMLTag() : m_pParentTag(NULL)
	{
	}

	~RXMLTag()
	{
		INT_PTR n;
		for (n = 0; n < m_childTags.GetSize(); n++)
			delete m_childTags[n];
		for (n = 0; n < m_properties.GetSize(); n++)
			delete m_properties[n];
	}

	const TCHAR* GetContent()
	{
		//if (m_strContent.IsEmpty())
		//	return NULL;
		return m_strContent; // a tag having an empty content is allowed
	}

	const TCHAR* GetName()
	{
		//if (m_strName.IsEmpty())
		//	return NULL;
		return m_strName;
	}

	RXMLTag* AddChild(const TCHAR* lpszName)
	{
		// Check name for invalid characters

		INT_PTR nNameLen = _tcslen(lpszName);
		for (INT_PTR n = 0; n < nNameLen; n++)
		{
			if (n == 0)
			{
				if (!(	*(lpszName + n) >= _T('A') && *(lpszName + n) <= _T('Z')	||
						*(lpszName + n) >= _T('a') && *(lpszName + n) <= _T('z')	||
						*(lpszName + n) == _T('_')									))
					return NULL;
			}
			else
			{
				if (!(	*(lpszName + n) >= _T('0') && *(lpszName + n) <= _T('9')	||
						*(lpszName + n) >= _T('A') && *(lpszName + n) <= _T('Z')	||
						*(lpszName + n) >= _T('a') && *(lpszName + n) <= _T('z')	||
						*(lpszName + n) == _T('_')									||
						*(lpszName + n) == _T('-')									||
						*(lpszName + n) == _T('.')									||
						*(lpszName + n) == _T(':')									))
					return NULL;
			}
		}

		if (_tcsnicmp(lpszName, _T("xml"), 3) == 0)
			return NULL;

		// Create new tag and add it to array

		RXMLTag* pNewTag = new RXMLTag;
		pNewTag->m_pParentTag = this;
		pNewTag->m_strName = lpszName;

		m_childTags.Add(pNewTag);

		return pNewTag;
	}

	bool SetName(const TCHAR* lpszName)
	{
		// Check name for invalid characters

		INT_PTR nNameLen = _tcslen(lpszName);
		for (INT_PTR n = 0; n < nNameLen; n++)
		{
			if (n == 0)
			{
				if (!(	*(lpszName + n) >= _T('A') && *(lpszName + n) <= _T('Z')	||
						*(lpszName + n) >= _T('a') && *(lpszName + n) <= _T('z')	||
						*(lpszName + n) == _T('_')									))
					return false;
			}
			else
			{
				if (!(	*(lpszName + n) >= _T('0') && *(lpszName + n) <= _T('9')	||
						*(lpszName + n) >= _T('A') && *(lpszName + n) <= _T('Z')	||
						*(lpszName + n) >= _T('a') && *(lpszName + n) <= _T('z')	||
						*(lpszName + n) == _T('_')									||
						*(lpszName + n) == _T('-')									||
						*(lpszName + n) == _T('.')									||
						*(lpszName + n) == _T(':')									))
					return false;
			}
		}

		if (_tcsnicmp(lpszName, _T("xml"), 3) == 0)
			return false;

		m_strName = lpszName;
		return true;
	}

	RXMLTag* GetChild(const TCHAR* lpszName)
	{
		if (!lpszName)
			return NULL;

		for (INT_PTR n = 0; n < m_childTags.GetSize(); n++)
			if (m_childTags[n]->m_strName == lpszName)
				return m_childTags[n];

		return NULL;
	}

	RXMLTag* GetChild(INT_PTR nIndex)
	{
		if (nIndex >= m_childTags.GetSize())
			return NULL;

		return m_childTags[nIndex];
	}

	const TCHAR* GetChildContent(const TCHAR* lpszName)
	{
		RXMLTag *pTag = GetChild(lpszName);
		if (pTag)
			return pTag->m_strContent;
		return NULL;
	}

	INT_PTR GetChildCount()
	{
		return m_childTags.GetSize();
	}

	void RemoveAllChildren()
	{
		for (INT_PTR n = 0; n < m_childTags.GetSize(); n++)
			delete m_childTags[n];
		m_childTags.SetSize(0);
	}

	bool SetProperty(const TCHAR* lpszName, const TCHAR* lpszValue)
	{
		ASSERT(lpszName);
		ASSERT(lpszValue);

		if (!lpszName || !lpszValue)
			return false;

		// Just assign a new value if property is already in array

		INT_PTR n;
		for (n = 0; n < m_properties.GetSize(); n += 2)
		{
			if (*m_properties[n] == lpszName)
			{
				*m_properties[n + 1] = lpszValue;
				return true;
			}
		}

		// Property not in array yet, first check the name for invalid characters
		
		INT_PTR nNameLen = _tcslen(lpszName);
		for (n = 0; n < nNameLen; n++)
		{
			if (n == 0)
			{
				if (!(	*(lpszName + n) >= _T('A') && *(lpszName + n) <= _T('Z')	||
						*(lpszName + n) >= _T('a') && *(lpszName + n) <= _T('z')	||
						*(lpszName + n) == _T('_')										))
					return false;
			}
			else
			{
				if (!(	*(lpszName + n) >= _T('0') && *(lpszName + n) <= _T('9')	||
						*(lpszName + n) >= _T('A') && *(lpszName + n) <= _T('Z')	||
						*(lpszName + n) >= _T('a') && *(lpszName + n) <= _T('z')	||
						*(lpszName + n) == _T('_')										||
						*(lpszName + n) == _T('-')										||
						*(lpszName + n) == _T('.')										))
					return false;
			}
		}

		// Add a new property to the array

		m_properties.Add(new RString(lpszName));
		m_properties.Add(new RString(lpszValue));

		return true;
	}

	const TCHAR* GetProperty(const TCHAR* lpszName)
	{
		if (!lpszName)
			return NULL;

		for (INT_PTR n = 0; n < m_properties.GetSize(); n += 2)
			if (*m_properties[n] == lpszName)
				return *m_properties[n + 1];

		return NULL;
	}

	void SetContent(const TCHAR* lpszValue)
	{
		m_strContent = lpszValue;
	}

protected:
	RXMLTag *m_pParentTag;
	RArray<RXMLTag*> m_childTags;
	RString m_strName, m_strContent;
	RArray<RString*> m_properties;
};

//
//
// RXMLFile class
//
//

class RXMLFile
{
public:
	RXMLFile() : m_charSet(XMLFILE_CHARACTERSET_UTF8)
	{
	}

	~RXMLFile()
	{
	}

	RXMLTag* GetRootTag()
	{
		return &m_rootTag;
	}

	bool Read(const TCHAR* lpszFilePath)
	{
		m_rootTag.RemoveAllChildren();

		// Open file and put it's data into a buffer

		/*
		HANDLE hFile = CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		DWORD dwSize = GetFileSize(hFile, NULL);
		BYTE* pReadBuf = new BYTE[dwSize + 2]; // two trailing zero bytes
		memset(pReadBuf, 0, dwSize + 2);
		DWORD dwRead;
		ReadFile(hFile, pReadBuf, dwSize, &dwRead, NULL);
		CloseHandle(hFile);

		// Convert the data correctly to string

		RString str;

		if (pReadBuf[0] == 0xEF && pReadBuf[1] == 0xBB && pReadBuf[2] == 0xBF)
		{
			m_charSet = XMLFILE_CHARACTERSET_UTF8;

			DWORD cch_plusone = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(pReadBuf + 3), -1, NULL, 0);
			WCHAR* pWideStr = new WCHAR[cch_plusone];
			MultiByteToWideChar(CP_UTF8, 0, (const char*)(pReadBuf + 3), -1, pWideStr, cch_plusone);
			str = pWideStr;
			delete[] pWideStr;
		}
		else if (pReadBuf[0] == 0xFF && pReadBuf[1] == 0xFE)
		{
			m_charSet = XMLFILE_CHARACTERSET_UNICODE;
			str = (const WCHAR*)(pReadBuf + 2);
		}
		else
		{
			m_charSet = XMLFILE_CHARACTERSET_ANSI;
			str = (const char*)pReadBuf;
		}

		delete[] pReadBuf;
		*/

		RString str;
		if (!FileToString(lpszFilePath, str))
			return false;

		ASSERT(!str.IsEmpty());

		// Parse the contents of the string

		INT_PTR nTagStart, nTagEnd, nNameStart, nNameEnd, nClosingTagStart, nClosingTagEnd, 
			nFirstQuote, nSecondQuote, i, j, k;
		RString strTagName, strPropertyName, strPropertyValue;
		RXMLTag *pTag, *pNewTag;
		
		pTag = &m_rootTag;

		for (i = 0; pTag && (i = str.Find(_T('<'), i)) != -1; i++)
		{
			if (str[i + 1] == _T('?') || 
				str[i + 1] == _T('!'))
				continue; // ignore comment tags
			
			if (str[i + 1] == _T('/'))
			{
				pTag = pTag->m_pParentTag; // go down one level
				continue;
			}

			nTagStart = i;

			// Find tag end

			nTagEnd = str.Find(_T('>'), nTagStart) + 1;
			if (nTagEnd == 0)
				goto read_error;

			// Get name of tag

			nNameStart = nTagStart + 1;
			nNameEnd = str.FindOneOf(_T(" \t\r\n"), nNameStart);
			if (nNameEnd == -1 ||
				nNameEnd >= nTagEnd)
				nNameEnd = nTagEnd - 1;

			strTagName = str.Mid(nNameStart, nNameEnd - nNameStart);
			if (strTagName.GetLength() && strTagName[strTagName.GetLength()-1] == _T('/'))
				strTagName = strTagName.Left(strTagName.GetLength() - 1);

			// Add new tag to array

			pNewTag = pTag->AddChild(strTagName);
			if (!pNewTag)
				goto read_error;

			i = nTagEnd - 1; // - 1 because of the i++ in the for-loop

			// Set tag properties

			for (j = nNameEnd; (j = str.Find(_T('='), j)) != -1; j++)
			{
				if (j >= nTagEnd)
					break; // found it, but not between tag begin and end

				for (k = j; k >= nNameEnd; k--) // reverse find space
				{
					if (str[k] == _T(' ') ||
						str[k] == _T('\t') ||
						str[k] == _T('\n'))
					{
						strPropertyName = str.Mid(k + 1, j - k - 1);
						break;
					}
				}

				if (str[j + 1] != _T('\"'))
					goto read_error; // property value should be between quotes
					
				nFirstQuote = j + 1;
				nSecondQuote = str.Find(_T('\"'), nFirstQuote + 1);
				if (nSecondQuote == -1 ||
					nSecondQuote >= nTagEnd)
					goto read_error; // can't find ending quote sign

				strPropertyValue = str.Mid(nFirstQuote + 1, nSecondQuote - nFirstQuote - 1);
				strPropertyValue.Replace(_T("&amp;"), _T("&"));
				strPropertyValue.Replace(_T("&lt;"), _T("<"));
				strPropertyValue.Replace(_T("&gt;"), _T(">"));
				strPropertyValue.Replace(_T("&quot;"), _T("\""));
				strPropertyValue.Replace(_T("&apos;"), _T("'"));

				if (!pNewTag->SetProperty(strPropertyName, strPropertyValue))
					goto read_error; // property name probably not valid

				j = nSecondQuote; // make sure it not catches a = sign between the quotes
			}

			// Single tag?

			if (str[nTagEnd - 2] == _T('/'))
				continue; // single tag

			// Find closing tag

			nClosingTagStart = str.Find(_T("</") + strTagName, nTagEnd);
			if (nClosingTagStart == -1)
				goto read_error; // no closing tag found

			nClosingTagEnd = str.Find(_T('>'), nClosingTagStart) + 1;
			if (nClosingTagEnd == 0)
				goto read_error; // end of closing tag not found

			// Does it have children or content?

			j = str.Find(_T('<'), nTagEnd); // should always find this because we already found nClosingTagStart
			if (j == nClosingTagStart) // has content
			{
				pNewTag->m_strContent = str.Mid(nTagEnd, nClosingTagStart - nTagEnd);

				bool bOnlySpaces = true;
				for (k = 0; k < pNewTag->m_strContent.GetLength(); k++)
				{
					if (pNewTag->m_strContent[k] != _T(' ') &&
						pNewTag->m_strContent[k] != _T('\t') &&
						pNewTag->m_strContent[k] != _T('\r') &&
						pNewTag->m_strContent[k] != _T('\n'))
					{
						bOnlySpaces = false;
						break;
					}
				}

				if (bOnlySpaces)
					pNewTag->m_strContent.Empty();
				else
				{
					pNewTag->m_strContent.Replace(_T("&amp;"), _T("&"));
					pNewTag->m_strContent.Replace(_T("&lt;"), _T("<"));
					pNewTag->m_strContent.Replace(_T("&gt;"), _T(">"));
					pNewTag->m_strContent.Replace(_T("&quot;"), _T("\""));
					pNewTag->m_strContent.Replace(_T("&apos;"), _T("'"));
				}

				i = nClosingTagEnd - 1; // - 1 because of the n++ in the for-loop
			}
			else
				pTag = pNewTag; // has children
		}
		
		return true;

read_error:
		TRACE(_T("RXMLFile::Open(...) Error while reading XML-file, tag name: \"%s\"\n"), (const TCHAR*)strTagName);
		m_rootTag.RemoveAllChildren();
		
		return false;
	}

	bool Write(const TCHAR* lpszFilePath)
	{
		// Generate header

		RString str(_T("<?xml version=\"1.0\" encoding=\""));
		if (m_charSet == XMLFILE_CHARACTERSET_UTF8)
			str += _T("UTF-8");
		else if (m_charSet == XMLFILE_CHARACTERSET_UNICODE)
			str += _T("UTF-16");
		else //if (m_charSet == XMLFILE_CHARACTERSET_ANSI)
			str += _T("Windows-1252");
		str += _T("\"?>\r\n");

		// Parse all tags to string

		for (INT_PTR i = 0; i < m_rootTag.m_childTags.GetSize(); i++)
			ParseToString(m_rootTag.m_childTags[i], 0, str);

		// Write string to file

		HANDLE hFile = CreateFile(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		DWORD dwWritten;
		if (m_charSet == XMLFILE_CHARACTERSET_UTF8)
		{
			WriteFile(hFile, "\xEF\xBB\xBF", 3, &dwWritten, NULL);
#ifdef _UNICODE
			int nBufLen = WideCharToMultiByte(CP_UTF8, 0, str, (int)str.GetLength(), NULL, 0, NULL, NULL);
			char *pBuffer = new char[nBufLen];
			WideCharToMultiByte(CP_UTF8, 0, str, -1, pBuffer, nBufLen, NULL, NULL);
			WriteFile(hFile, pBuffer, nBufLen, &dwWritten, NULL);
			delete[] pBuffer;
#else
			WriteFile(hFile, str, str.GetLength(), &dwWritten, NULL);
#endif
		}
		else if (m_charSet == XMLFILE_CHARACTERSET_UNICODE)
		{
			WriteFile(hFile, "\xFF\xFE", 2, &dwWritten, NULL);
#ifdef _UNICODE
			WriteFile(hFile, str, (DWORD)str.GetLength() * sizeof(TCHAR), &dwWritten, NULL);
#else
			WCHAR* pWide = new WCHAR[str.GetLength()];
			MultiByteToWideChar(CP_ACP, 0, str, -1, pWide, str.GetLength());
			WriteFile(hFile, pWide, str.GetLength() * sizeof(WCHAR), &dwWritten, NULL);
			delete[] pWide;
#endif
		}
		else //if (m_charSet == XMLFILE_CHARACTERSET_ANSI)
		{
#ifdef _UNICODE
			char* pByte = new char[str.GetLength()];
			WideCharToMultiByte(CP_ACP, 0, str, -1, pByte, (int)str.GetLength(), NULL, NULL);
			WriteFile(hFile, pByte, (DWORD)str.GetLength(), &dwWritten, NULL);
			delete[] pByte;
#else
			WriteFile(hFile, str, str.GetLength(), &dwWritten, NULL);
#endif
		}

		CloseHandle(hFile);
		return true;
	}

protected:
	static bool ParseToString(RXMLTag *pTag, INT_PTR nLevel, RString& str)
	{
		if (!pTag)
			return false;

		RString strSpaces;
		INT_PTR i;
		for (i = 0; i < nLevel; i++)
			strSpaces += _T("\t"); //_T("    ");

		bool bHasChildren = pTag->m_childTags.GetSize() != 0;
		bool bHasContent = pTag->m_strContent.GetLength() != 0;

		str += strSpaces + _T("<") + pTag->m_strName;

		for (i = 0; i < pTag->m_properties.GetSize(); i += 2)
		{
			pTag->m_properties[i + 1]->Replace(_T("&"), _T("&amp;"));
			pTag->m_properties[i + 1]->Replace(_T("<"), _T("&lt;"));
			pTag->m_properties[i + 1]->Replace(_T(">"), _T("&gt;"));
			pTag->m_properties[i + 1]->Replace(_T("\""), _T("&quot;"));
			pTag->m_properties[i + 1]->Replace(_T("'"), _T("&apos;"));

			str += _T(" ") + *pTag->m_properties[i] + _T("=\"") + *pTag->m_properties[i + 1] + _T("\"");
		}

		if (bHasChildren)
		{
			// parse children of this tag
			str += _T(">\r\n");
			for (i = 0; i < pTag->m_childTags.GetSize(); i++)
				ParseToString(pTag->m_childTags[i], nLevel + 1, str);
			str += strSpaces + _T("</") + pTag->m_strName + _T(">\r\n");
		}
		else if (bHasContent)
		{
			pTag->m_strContent.Replace(_T("&"), _T("&amp;"));
			pTag->m_strContent.Replace(_T("<"), _T("&lt;"));
			pTag->m_strContent.Replace(_T(">"), _T("&gt;"));
			pTag->m_strContent.Replace(_T("\""), _T("&quot;"));
			pTag->m_strContent.Replace(_T("'"), _T("&apos;"));

			str += _T(">") + pTag->m_strContent + _T("</") + pTag->m_strName + _T(">\r\n");
		}
		else
		{
			str += _T(" />\r\n");
			//str += _T("></") + pTag->m_strName + _T(">\r\n");
		}

		return true;
	}
	RXMLTag m_rootTag;
	UINT m_charSet;
};

#endif // __RXML_H__