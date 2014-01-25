#ifndef __RXML2_H__
#define __RXML2_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
// RXMLElem2

class RXMLElem2
{
	friend class RXMLFile2;

public:
	RXMLElem2(RXMLElem2 *pParent) : m_pParent(pParent)
	{
	}

	~RXMLElem2()
	{
		RemoveAllChildren();
	}

	RString_ GetName() const
	{
		return m_sName;
	}

	template <class S>
	bool SetName(S&& sName)
	{
		if (!ValidateName(sName))
			ASSERTRETURN(false);
		m_sName = forw<S>(sName);
		return true;
	}

	RString_ GetContent() const
	{
		return m_sContent;
	}

	template <class S>
	void SetContent(S&& sContent)
	{
		m_sContent = forw<S>(sContent);
	}

	RXMLElem2* GetParent() const
	{
		return m_pParent;
	}

	template <class S>
	RXMLElem2* GetChild(S&& sName)
	{
		for (INT_PTR i = 0; i != m_children; ++i)
			if (m_children[i]->m_sName == sName)
				return m_children[i];
		//ASSERT(false); function is used to test if child exists, do not assert
		return NULL;
	}

	template <class S>
	const RXMLElem2* GetChild(S&& sName) const
		{return ((RXMLElem2*)this)->GetChild(sName);}

	template <class S>
	RXMLElem2* AddChild(S&& sName)
	{
		if (!ValidateName(sName))
			{ASSERT(false); return NULL;}
		RXMLElem2 *pChild = new RXMLElem2(this);
		pChild->m_sName = forw<S>(sName);
		m_children.Add(pChild);
		return pChild;
	}

	template <class S>
	RString_ GetChildContent(S&& sName) const
	{
		for (INT_PTR i = 0; i != m_children; ++i)
			if (m_children[i]->m_sName == sName)
				return m_children[i]->m_sContent;
		ASSERT(false);
		return GetEmptyStr();
	}

	template <class S1, class S2>
	bool SetChildContent(S1&& sName, S2&& sContent, bool bCreate = false)
	{
		RXMLElem2 *pElem = GetChild(sName);
		if (!pElem)
		{
			if (!bCreate)
				return false;
			pElem = AddChild(forw<S1>(sName));
			if (!pElem)
				ASSERTRETURN(false);
		}
		pElem->SetContent(forw<S2>(sContent));
		return true;
	}

	template <class S>
	RString_ GetAttribute(S&& sName) const
	{
		for (INT_PTR i = 0; i != m_attributes; ++i)
			if (m_attributes[i].sName == sName)
				return m_attributes[i].sValue;
		//ASSERT(false);
		return GetEmptyStr();
	}

	template <class S1, class S2>
	bool AddAttribute(S1&& sName, S2&& sValue)
	{
		if (!ValidateName(sName))
			ASSERTRETURN(false);
		ATTRIBUTE *pAttr = m_attributes.AddNew();
		pAttr->sName = forw<S1>(sName);
		pAttr->sValue = forw<S2>(sValue);
		return true;
	}

	template <class S1, class S2>
	bool SetAttribute(S1&& sName, S2&& sValue, bool bCreate = false)
	{
		for (INT_PTR i = 0; i < m_attributes; ++i)
			if (m_attributes[i].sName == sName)
				{m_attributes[i].sValue = forw<S2>(sValue); return true;}
		if (bCreate)
			return AddAttribute(forw<S1>(sName), forw<S2>(sValue));
		return false;
	}

	const RArray<RXMLElem2*>& GetChildren() const
	{
		return m_children;
	}

	void RemoveAllChildren()
	{
		for (INT_PTR i = 0; i < m_children; ++i)
			delete m_children[i];
		m_children.SetSize(0);
	}

	void RemoveAllAttributes()
	{
		m_attributes.SetSize(0);
	}

private:
	static bool ValidateName(const TCHAR *lpszName)
	{
		return (lpszName && *lpszName && _tcspbrk(lpszName, _T(" &?!<>/='\"")) == NULL);
	}

	RXMLElem2 *m_pParent;
	RString m_sName, m_sContent;
	RArray<RXMLElem2*> m_children;
	struct ATTRIBUTE {RString sName, sValue;};
	RObArray<ATTRIBUTE> m_attributes;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// RXMLFile2

class RXMLFile2
{
public:
	RXMLFile2() : m_rootElem(NULL)
	{
	}

	~RXMLFile2()
	{
		Close();
	}

	template <class S>
	bool Open(S&& sFilePath)
	{
		Close();
		
		// Read file to string

		RString sFile;
		if (!FileToString(sFilePath, sFile))
			return false;

		ASSERT(!sFile.IsEmpty()); // empty XML-file, is this the intention?

		m_sFilePath = forw<S>(sFilePath);

		// Parse file contents

		const TCHAR *p1, *p2 = sFile, *p3, *p4;
		RXMLElem2 *pElem = NULL;
		RString sName, sValue;

		for (;;)
		{
			p1 = _tcschr(p2, _T('<'));
			if (!p1)
				break;

			p2 = _tcschr(p1, _T('>'));
			if (!p2)
				{ASSERT(false); goto parse_error;} // tag has no end

			if (p2 == p1+1)
				{ASSERT(false); goto parse_error;} // tag has no name

			// Declaration tag

			if (*(p1+1) == _T('?'))
			{
				if (*(p2-1) != _T('?'))
					{ASSERT(false); goto parse_error;} // invalid declaration element

				// TODO: Parse declaration element

				// ...
			}

			// Comment tag

			else if (*(p1+1) == _T('!'))
			{
				if (p2 - p1 < 6 || *(p1+2) != _T('-') || *(p1+3) != _T('-') ||
						*(p2-1) != _T('-') || *(p2-2) != _T('-'))
					{ASSERT(false); goto parse_error;} // invalid comment element

				// TODO: Parse comment element

				// ...
			}

			// 	End tag of regular element, go down one level

			else if (*(p1+1) == _T('/'))
			{
				if (!pElem)
					{ASSERT(false); goto parse_error;} // cannot start file with closing tag

				if (p2-p1-2 != pElem->m_sName.GetLength() ||
						_tcsncmp(p1+2, pElem->m_sName, pElem->m_sName.GetLength()) != 0)
					{ASSERT(false); goto parse_error;} // ending tag should have same name

				pElem = pElem->m_pParent;
			}

			// Start tag of regular element, go up one level

			else
			{
				// Extract element name

				p3 = _tcschr(p1, _T(' '));
				if (!p3 || p3 > p2)
					p3 = p2;

				sName = sFile.Mid(p1+1 - (LPCTSTR)sFile, p3-p1-1);

				// Add the new child element

				if (pElem)
				{
					pElem = pElem->AddChild(move(sName));
					if (!pElem)
						{ASSERT(false); goto parse_error;} // failed to add child
				}

				// Or this is the root element

				else
				{
					if (!m_rootElem.m_sName.IsEmpty())
						{ASSERT(false); goto parse_error;} // there can only be one root element

					pElem = &m_rootElem;
					if (!pElem->SetName(move(sName)))
						{ASSERT(false); goto parse_error;} // invalid name for root element
				}

				// Parse its attributes

				while (*p3 == _T(' '))
				{
					while (*p3 == _T(' '))
						++p3;

					p4 = _tcschr(p3, _T('='));
					if (!p4 || p4 > p2)
						break;

					sName = sFile.Mid(p3 - (LPCTSTR)sFile, p4-p3);

					if (*(p4+1) != _T('"'))
						{ASSERT(false); goto parse_error;} // value should be between quotes

					p3 = _tcschr(p4+2, _T('"'));
					if (!p3 || p3 > p2)
						{ASSERT(false); goto parse_error;} // could not find end quote

					sValue = sFile.Mid(p4+2 - (LPCTSTR)sFile, p3-p4-2);
					DecodeValue(&sValue);

					if (!pElem->AddAttribute(move(sName), move(sValue)))
						{ASSERT(false); goto parse_error;} // failed to set attribute

					++p3;
				}
				
				// If this is a single tag element go down one level

				if (*(p2-1) == _T('/'))
					pElem = pElem->m_pParent;

				// Otherwise check whether this tag has content

				else
				{
					p3 = _tcschr(p2, _T('<'));

					if (!p3 || p3+pElem->m_sName.GetLength()+2 >= (LPCTSTR)sFile + sFile.GetLength())
						{ASSERT(false); goto parse_error;} // ending tag probably missing
					
					if (*(p3+1) == _T('/') && *(p3+pElem->m_sName.GetLength()+2) == _T('>') &&
							_tcsncmp(p3+2, pElem->m_sName, pElem->m_sName.GetLength()) == 0)
					{
						pElem->m_sContent = sFile.Mid(p2+1 - (LPCTSTR)sFile, p3-p2-1);
						DecodeValue(&pElem->m_sContent);

						p2 = p3+pElem->m_sName.GetLength()+3;

						pElem = pElem->m_pParent;
					}
				}
			}
		}

		return true;

parse_error:
		Close();
		return false;
	}

	void Close()
	{
		m_sFilePath.Empty();

		m_rootElem.m_sName.Empty();
		m_rootElem.m_sContent.Empty();
		m_rootElem.RemoveAllAttributes();
		m_rootElem.RemoveAllChildren();
	}

	template <class S>
	bool SaveAs(S&& sFilePath)
	{
		m_sFilePath = forw<S>(sFilePath);
		if (m_sFilePath.IsEmpty())
			return false;

		RString sFile = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
		ElemToStr(m_rootElem, &sFile, 0);

		return StringToFile(sFile, m_sFilePath);
	}

	bool Save()
	{
		return SaveAs(m_sFilePath);
	}

	RXMLElem2& GetRootElem() {return m_rootElem;}
	const RXMLElem2& GetRootElem() const {return m_rootElem;}
	__declspec(property(get = GetRootElem)) RXMLElem2 rootElem;

private:
	static void EncodeValue(RString *pStr)
	{
		ASSERT(pStr);
		pStr->Replace(_T("&"), _T("&amp;"));
		pStr->Replace(_T("<"), _T("&lt;"));
		pStr->Replace(_T(">"), _T("&gt;"));
		pStr->Replace(_T("'"), _T("&apos;"));
		pStr->Replace(_T("\""), _T("&quot;"));
	}

	static void DecodeValue(RString *pStr)
	{
		ASSERT(pStr);
		pStr->Replace(_T("&quot;"), _T("\""));
		pStr->Replace(_T("&apos;"), _T("'"));
		pStr->Replace(_T("&gt;"), _T(">"));
		pStr->Replace(_T("&lt;"), _T("<"));
		pStr->Replace(_T("&amp;"), _T("&"));
	}

	static void ElemToStr(const RXMLElem2& elem, RString *psFile, INT_PTR nLevel)
	{
		INT_PTR i;
		RString sIndent, sTemp;

		for (i = 0; i != nLevel; ++i)
			sIndent += _T("\t");

		*psFile += sIndent + _T("<") + elem.m_sName;

		for (i = 0; i != elem.m_attributes; ++i)
		{
			sTemp = elem.m_attributes[i].sValue;
			EncodeValue(&sTemp);
			*psFile += _T(" ") + elem.m_attributes[i].sName + _T("=\"") + sTemp + _T("\"");
		}

		if (elem.m_children)
		{
			*psFile += _T(">\r\n");
			for (i = 0; i != elem.m_children; ++i)
				ElemToStr(*elem.m_children[i], psFile, nLevel + 1);
			*psFile += sIndent + _T("</") + elem.m_sName + _T(">\r\n");
		}
		else if (!elem.m_sContent.IsEmpty())
		{
			sTemp = elem.m_sContent;
			EncodeValue(&sTemp);
			*psFile += _T(">") + sTemp + _T("</") + elem.m_sName + _T(">\r\n");
		}
		else
			*psFile += _T(" />\r\n");
	}

	RString m_sFilePath;
	RXMLElem2 m_rootElem;
};

#endif // __RXML2_H__
