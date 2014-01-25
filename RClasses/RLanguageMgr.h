#ifndef __RLANGUAGEMGR_H__
#define __RLANGUAGEMGR_H__

#define GETSTR(x) GetLangMgr()->GetString(x-1000)

class RLanguageMgr
{
public:
	RLanguageMgr() : m_nSelectedLanguage(-1)
	{
	}

	RLanguageMgr(RString_ strLanguagesDir) : m_nSelectedLanguage(-1)
	{
		Load(strLanguagesDir);
	}

	virtual ~RLanguageMgr()
	{
		Unload();
	}

	bool Load(RString_ strLanguagesDir)
	{
		Unload();

		if (strLanguagesDir.IsEmpty())
			{ASSERT(false); return false;}

		m_strLanguagesDir = strLanguagesDir;

		WIN32_FIND_DATA ffd;
		HANDLE hFindFile = FindFirstFile(strLanguagesDir + _T("\\*.*"), &ffd);
		
		if (hFindFile == INVALID_HANDLE_VALUE)
			return false;

		RString strFileName, strTagName;
		RXMLFile xmlFile;

		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			strFileName = ffd.cFileName;
			strFileName.MakeLower();

			if (strFileName.GetLength() <= 3 || strFileName.Right(3) != _T("xml"))
				continue;

			if (!xmlFile.Read(strLanguagesDir + _T("\\") + strFileName))
				{ASSERT(false); continue;}

			RXMLTag *pTag = xmlFile.GetRootTag()->GetChild(_T("LanguageFile"));
			if (!pTag)
				{ASSERT(false); continue;}

			LANGUAGE *pLang = new LANGUAGE;
			pLang->strEnglishName = pTag->GetChildContent(_T("EnglishName"));
			pLang->strName = pTag->GetChildContent(_T("Name"));
			pLang->strAuthor = pTag->GetChildContent(_T("Author"));

			RXMLTag *pStringsTag = pTag->GetChild(_T("Strings"));
			if (!pStringsTag)
				{delete pLang; ASSERT(false); continue;}

			INT_PTR nIndex;
			for (INT_PTR i = 0; i < pStringsTag->GetChildCount(); i++)
			{
				pTag = pStringsTag->GetChild(i);
				strTagName = pTag->GetName();

				if (strTagName.GetLength() <= 1 || strTagName.Left(1) != _T("S"))
					{ASSERT(false); continue;}
				nIndex = StringToNumber((LPCTSTR)strTagName + 1) - 1;

				while (nIndex >= pLang->m_strings)
					pLang->m_strings.Add(new RString);
				
				*pLang->m_strings[nIndex] = pTag->GetContent();
			}

			AddLanguage(pLang);

		} while (FindNextFile(hFindFile, &ffd));

		FindClose(hFindFile);
		
		return true;
	}

	void Unload()
	{
		INT_PTR i, j;
		for (i = 0; i < m_languages; i++)
		{
			for (j = 0; j < m_languages[i]->m_strings; j++)
				delete m_languages[i]->m_strings[j];
			delete m_languages[i];
		}
		m_languages.SetSize(0);
		m_nSelectedLanguage = -1;
	}

	bool Save(RString strLanguagesDir = (LPCTSTR)NULL)
	{
		if (strLanguagesDir.IsEmpty())
			strLanguagesDir = m_strLanguagesDir;

		if (strLanguagesDir.IsEmpty())
			{ASSERT(false); return false;}

		CreateDirectory(strLanguagesDir, NULL);

		RXMLFile xmlFile;
		RXMLTag *pTag;
		TCHAR buf[10];

		for (INT_PTR i = 0; i < m_languages; i++)
		{
			xmlFile.GetRootTag()->RemoveAllChildren();
			pTag = xmlFile.GetRootTag()->AddChild(_T("LanguageFile"));
			pTag->SetProperty(_T("applicationID"), APPLICATION_ID);
			pTag->AddChild(_T("EnglishName"))->SetContent(m_languages[i]->strEnglishName);
			pTag->AddChild(_T("Name"))->SetContent(m_languages[i]->strName);
			pTag->AddChild(_T("Author"))->SetContent(m_languages[i]->strAuthor);
			
			pTag = pTag->AddChild(_T("Strings"));
			for (INT_PTR j = 0; j < m_languages[i]->m_strings; j++)
			{
				_stprintf_s(buf, 10, _T("S%.4d"), j+1);
				pTag->AddChild(buf)->SetContent(*m_languages[i]->m_strings[j]);
			}

			if (!xmlFile.Write(strLanguagesDir + _T("\\") + m_languages[i]->strEnglishName + _T(".xml")))
				{ASSERT(false); return false;}
		}
		return true;
	}

	INT_PTR AddLanguage(RString_ strEnglishName, RString_ strName, RString_ strAuthor)
	{
		if (GetLanguage(strEnglishName) != -1)
			{ASSERT(false); return -1;}

		LANGUAGE *pLang = new LANGUAGE;
		pLang->strEnglishName = strEnglishName;
		pLang->strName = strName;
		pLang->strAuthor = strAuthor;

		if (m_nSelectedLanguage == -1)
			m_nSelectedLanguage = 0;

		return AddLanguage(pLang);
	}

	bool SetLanguage(INT_PTR nLanguage, RString_ strEnglishName, RString_ strName, RString_ strAuthor)
	{
		if (nLanguage < 0 || nLanguage >= m_languages)
			{ASSERT(false); return false;}

		m_languages[nLanguage]->strEnglishName = strEnglishName;
		m_languages[nLanguage]->strName = strName;
		m_languages[nLanguage]->strAuthor = strAuthor;

		return true;
	}

	bool GetLanguage(INT_PTR nLanguage, RString &strEnglishName, RString &strName, RString &strAuthor)
	{
		if (nLanguage < 0 || nLanguage >= m_languages)
			{ASSERT(false); return false;}

		strEnglishName = m_languages[nLanguage]->strEnglishName;
		strName = m_languages[nLanguage]->strName;
		strAuthor = m_languages[nLanguage]->strAuthor;

		return true;
	}

	INT_PTR GetLanguage(RString_ strEnglishName)
	{
		for (INT_PTR i = 0; i < m_languages; i++)
			if (_tcsicmp(m_languages[i]->strEnglishName, strEnglishName) == 0)
				return i;
		return -1;
	}

	INT_PTR GetLanguageCount()
	{
		return m_languages;
	}

	bool SelectLanguage(INT_PTR nLanguage)
	{
		if (nLanguage < 0 || nLanguage >= m_languages)
			{ASSERT(false); return false;}

		m_nSelectedLanguage = nLanguage;
		return true;
	}

	bool SelectLanguage(RString_ strEnglishName)
	{
		m_nSelectedLanguage = GetLanguage(strEnglishName);
		return m_nSelectedLanguage != -1;
	}

	INT_PTR GetSelectedLanguage()
	{
		return m_nSelectedLanguage;
	}

	bool SetString(INT_PTR nLanguage, INT_PTR nIndex, RString_ str, bool bOverwrite = true)
	{
		if (nLanguage < 0 || nLanguage >= m_languages || nIndex < 0)
			{ASSERT(false); return false;}

		//while (nIndex >= m_languages[nLanguage]->m_strings)
		//	m_languages[nLanguage]->m_strings.Add(new RString);

		if (nIndex >= m_languages[nLanguage]->m_strings)
			m_languages[nLanguage]->m_strings.Add(new RString);
		ASSERT(nIndex < m_languages[nLanguage]->m_strings);

		if (bOverwrite || m_languages[nLanguage]->m_strings[nIndex]->IsEmpty())
			*m_languages[nLanguage]->m_strings[nIndex] = str;

		return true;
	}

	RString GetString(INT_PTR nLanguage, INT_PTR nIndex)
	{
		if (nLanguage >= 0 && nLanguage < m_languages &&
				nIndex >= 0 && nIndex < m_languages[nLanguage]->m_strings)
			return *m_languages[nLanguage]->m_strings[nIndex];
		ASSERT(false);
		return RString();
	}

	RString GetString(INT_PTR nIndex)
	{
		if (m_nSelectedLanguage >= 0 && m_nSelectedLanguage < m_languages && 
				nIndex >= 0 && nIndex < m_languages[m_nSelectedLanguage]->m_strings)
			return *m_languages[m_nSelectedLanguage]->m_strings[nIndex];
		ASSERT(false);
		return RString();
	}

	INT_PTR GetStringCount(INT_PTR nLanguage)
	{
		if (nLanguage >= 0 && nLanguage < m_languages)
			return m_languages[nLanguage]->m_strings;
		ASSERT(false);
		return -1;
	}

	INT_PTR GetStringCount()
	{
		if (m_nSelectedLanguage >= 0 && m_nSelectedLanguage < m_languages)
			return m_languages[m_nSelectedLanguage]->m_strings;
		ASSERT(false);
		return -1;
	}

protected:
	struct LANGUAGE
	{
		RString strEnglishName, strName, strAuthor;
		RArray<RString*> m_strings;
	};

	INT_PTR AddLanguage(LANGUAGE *pLang)
	{
		// Always put English on top
		
		if (_tcsicmp(pLang->strEnglishName, _T("English")) == 0)
		{
			m_languages.InsertAt(0, pLang);
			return 0;
		}

		// Otherwise insert it at the correct position, based on strName, alphabetically

		INT_PTR i, j, n;
		RString str1 = pLang->strName;
		str1.MakeLower();
		RString str2;

		for (i = 0; i < m_languages; i++)
		{
			if (i == 0 && _tcsicmp(m_languages[i]->strEnglishName, _T("English")) == 0)
				continue;

			str2 = m_languages[i]->strName;
			str2.MakeLower();

			n = min(str1.GetLength(), str2.GetLength());

			for (j = 0; j < n; j++)
			{
				if (str1[j] < str2[j])
					return m_languages.InsertAt(i, pLang);
				if (str1[j] > str2[j])
					break;
			}
		}

		return m_languages.Add(pLang);
	}

	RArray<LANGUAGE*> m_languages;
	INT_PTR m_nSelectedLanguage;
	RString m_strLanguagesDir;
};

#endif // __RLANGUAGEMGR_H__
