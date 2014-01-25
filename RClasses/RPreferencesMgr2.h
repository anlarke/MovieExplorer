#ifndef __RPREFERENCESMGR2_H__
#define __RPREFERENCESMGR2_H__

#define GETPREFSTR		GetPrefMgr()->GetStr
#define SETPREFSTR		GetPrefMgr()->SetStr
#define GETPREFINT		GetPrefMgr()->GetInt
#define SETPREFINT		GetPrefMgr()->SetInt
#define GETPREFBOOL		GetPrefMgr()->GetBool
#define SETPREFBOOL		GetPrefMgr()->SetBool
#define GETPREFFLOAT	GetPrefMgr()->GetFloat
#define SETPREFFLOAT	GetPrefMgr()->SetFloat
#define GETPREFCLR		GetPrefMgr()->GetClr
#define SETPREFCLR		GetPrefMgr()->SetClr

class RPreferencesMgr2
{
public:
	RPreferencesMgr2()
		{}

	~RPreferencesMgr2()
		{Close();}

	bool Open(RString sFilePath, RString sApplicationID = NULL, 
			RString sRootElemName = _T("PreferencesFile"))
	{
		Close();

		if (!m_xmlFile.Open(sFilePath))
			return false;

		if (sApplicationID.IsEmpty())
			sApplicationID = m_xmlFile.rootElem.GetAttribute(_T("applicationID"));
		else if (sApplicationID != m_xmlFile.rootElem.GetAttribute(_T("applicationID")))
			{Close(); return false;}

		if (sRootElemName.IsEmpty())
			sRootElemName = m_xmlFile.rootElem.GetName();
		else if (sRootElemName != m_xmlFile.rootElem.GetName())
			{Close(); return false;}

		m_sFilePath = move(sFilePath);
		m_sApplicationID = move(sApplicationID);
		m_sRootElemName = move(sRootElemName);

		return true;
	}

	void Close(bool bSave = false)
	{
		if (bSave)
			Save();
		m_xmlFile.rootElem.RemoveAllChildren();
		m_xmlFile.rootElem.RemoveAllAttributes();
		m_sFilePath.Empty();
		m_sApplicationID.Empty();
		m_sRootElemName.Empty();
	}

	bool Save()
		{return SaveAs(move(m_sFilePath), move(m_sApplicationID), move(m_sRootElemName));}

	bool SaveAs(RString sFilePath, RString sApplicationID = NULL, 
			RString sRootElemName = _T("PreferencesFile"))
	{
		m_sFilePath = move(sFilePath);
		m_sApplicationID = move(sApplicationID);
		m_sRootElemName = move(sRootElemName);

		if (m_sFilePath.IsEmpty())
			ASSERTRETURN(false);

		if (m_sRootElemName.IsEmpty())
			ASSERTRETURN(false);

		if (!m_xmlFile.rootElem.SetName(m_sRootElemName))
			ASSERTRETURN(false);

		if (!m_sApplicationID.IsEmpty() && !m_xmlFile.rootElem.SetAttribute(
				_T("applicationID"), m_sApplicationID))
			ASSERTRETURN(false);

		if (!m_xmlFile.SaveAs(m_sFilePath))
			ASSERTRETURN(false);

		return true;
	}

	//
	// String GET-functions
	//

	template <class S>
	RString_ GetStr(S&& sName) const
	{
		ASSERT(m_xmlFile.rootElem.GetChild(sName));
		return m_xmlFile.rootElem.GetChildContent(forw<S>(sName));
	}

	template <class S1, class S2>
	RString_ GetStr(S1&& sCategory, S2&& sName) const
	{
		const RXMLElem2 *pElem = m_xmlFile.rootElem.GetChild(forw<S1>(sCategory));
		if (pElem)
		{
			ASSERT(pElem->GetChild(sName));
			return pElem->GetChildContent(forw<S2>(sName));
		}
		ASSERT(false);
		return GetEmptyStr();
	}

	template <class S1, class S2, class S3>
	RString_ GetStr(S1&& sCategory, S2&& sSubCategory, S3&& sName) const
	{
		const RXMLElem2 *pElem = m_xmlFile.rootElem.GetChild(forw<S1>(sCategory));
		if (pElem)
		{
			pElem = pElem->GetChild(forw<S2>(sSubCategory));
			if (pElem)
			{
				ASSERT(pElem->GetChild(sName));
				return pElem->GetChildContent(forw<S3>(sName));
			}
		}
		ASSERT(false);
		return GetEmptyStr();
	}

	//
	// String SET-functions
	//

	template <class S1, class S2>
	bool SetStr(S1&& sName, S2&& sValue, bool bOverwrite = true)
	{
		if (!bOverwrite && m_xmlFile.rootElem.GetChild(sName))
			return false;
		ASSERTRETURN(m_xmlFile.rootElem.SetChildContent(forw<S1>(sName), forw<S2>(sValue), true));
	}

	template <class S1, class S2, class S3>
	bool SetStr(S1&& sCategory, S2&& sName, S3&& sValue, bool bOverwrite = true)
	{
		RXMLElem2 *pElem = m_xmlFile.rootElem.GetChild(sCategory);
		if (!pElem)
		{
			pElem = m_xmlFile.rootElem.AddChild(forw<S1>(sCategory));
			if (!pElem)
				ASSERTRETURN(false);
		}
		if (!bOverwrite && pElem->GetChild(sName))
			return false;
		ASSERTRETURN(pElem->SetChildContent(forw<S2>(sName), forw<S3>(sValue), true));
	}

	template <class S1, class S2, class S3, class S4>
	bool SetStr(S1&& sCategory, S2&& sSubCategory, S3&& sName, 
			S4&& sValue, bool bOverwrite = true)
	{
		RXMLElem2 *pElem = m_xmlFile.rootElem.GetChild(sCategory);
		if (!pElem)
		{
			pElem = m_xmlFile.rootElem.AddChild(forw<S1>(sCategory));
			if (!pElem)
				ASSERTRETURN(false);
		}
		pElem = pElem->GetChild(sSubCategory);
		if (!pElem)
		{
			pElem = pElem->AddChild(forw<S2>(sSubCategory));
			if (!pElem)
				ASSERTRETURN(false);
		}
		if (!bOverwrite && pElem->GetChild(sName))
			return false;
		ASSERTRETURN(pElem->SetChildContent(forw<S3>(sName), forw<S4>(sValue), true));
	}

	//
	// Integer GET-functions
	//

	template <class S>
	int GetInt(S&& sName) const
		{return _ttoi(GetStr(forw<S>(sName)));}

	template <class S1, class S2>
	int GetInt(S1&& sCategory, S2&& sName) const
		{return _ttoi(GetStr(forw<S1>(sCategory), forw<S2>(sName)));}

	template <class S1, class S2, class S3>
	int GetInt(S1&& sCategory, S2&& sSubCategory, S3&& sName) const
		{return _ttoi(GetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), forw<S3>(sName)));}

	//
	// Integer SET-functions
	//

	template <class S>
	bool SetInt(S&& sName, int nValue, bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf(sValue.GetBuffer(15), _T("%d"), nValue));
		return SetStr(forw<S>(sName), move(sValue), bOverwrite);
	}

	template <class S1, class S2>
	bool SetInt(S1&& sCategory, S2&& sName, int nValue, bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf(sValue.GetBuffer(15), _T("%d"), nValue));
		return SetStr(forw<S1>(sCategory), forw<S2>(sName), move(sValue), bOverwrite);
	}

	template <class S1, class S2, class S3>
	bool SetInt(S1&& sCategory, S2&& sSubCategory, S3&& sName, int nValue, bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf(sValue.GetBuffer(15), _T("%d"), nValue));
		return SetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), forw<S3>(sName), 
				move(sValue), bOverwrite);
	}

	//
	// Boolean GET-functions
	//

	template <class S>
	bool GetBool(S&& sName) const
		{return GetStr(forw<S>(sName)) == _T("true");}

	template <class S1, class S2>
	bool GetBool(S1&& sCategory, S2&& sName) const
		{return GetStr(forw<S1>(sCategory), forw<S2>(sName)) == _T("true");}

	template <class S1, class S2, class S3>
	bool GetBool(S1&& sCategory, S2&& sSubCategory, S3&& sName) const
		{return GetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), 
				forw<S3>(sName)) == _T("true");}

	//
	// Boolean SET-functions
	//

	template <class S>
	bool SetBool(S&& sName, bool bValue, bool bOverwrite = true)
		{return SetStr(forw<S>(sName), (bValue ? _T("true") : _T("false")), bOverwrite);}

	template <class S1, class S2>
	bool SetBool(S1&& sCategory, S2&& sName, bool bValue, bool bOverwrite = true)
		{return SetStr(forw<S1>(sCategory), forw<S2>(sName), (bValue ? _T("true") : 
				_T("false")), bOverwrite);}

	template <class S1, class S2, class S3>
	bool SetBool(S1&& sCategory, S2&& sSubCategory, S3&& sName, bool bValue, bool bOverwrite = true)
		{return SetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), forw<S3>(sName), 
				(bValue ? _T("true") : _T("false")), bOverwrite);}

	//
	// Floating point GET-functions
	//

	template <class S>
	float GetFloat(S&& sName) const
	{
		float fValue = 0.0;
		VERIFY(_stscanf_l(GetStr(forw<S>(sName)), _T("%g"), GetLocaleC(), &fValue) == 1);
		return fValue;
	}

	template <class S1, class S2>
	float GetFloat(S1&& sCategory, S2&& sName) const
	{
		float fValue = 0.0;
		VERIFY(_stscanf_l(GetStr(forw<S1>(sCategory), forw<S2>(sName)), _T("%g"), 
				GetLocaleC(), &fValue) == 1);
		return fValue;
	}

	template <class S1, class S2, class S3>
	float GetFloat(S1&& sCategory, S2&& sSubCategory, S3&& sName) const
	{
		float fValue = 0.0;
		VERIFY(_stscanf_l(GetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), forw<S3>(sName)), 
				_T("%g"), GetLocaleC(), &fValue) == 1);
		return fValue;
	}

	//
	// Floating point SET-functions
	//

	template <class S>
	bool SetFloat(S&& sName, float fValue, bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf_l(sValue.GetBuffer(31), _T("%g"), 
				GetLocaleC(), (double)fValue));
		return SetStr(forw<S>(sName), move(sValue), bOverwrite);
	}

	template <class S1, class S2>
	bool SetFloat(S1&& sCategory, S2&& sName, float fValue, bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf_l(sValue.GetBuffer(31), _T("%g"), 
				GetLocaleC(), (double)fValue));
		return SetStr(forw<S1>(sCategory), forw<S2>(sName), move(sValue), bOverwrite);
	}

	template <class S1, class S2, class S3>
	bool SetFloat(S1&& sCategory, S2&& sSubCategory, S3&& sName, float fValue, 
			bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf_l(sValue.GetBuffer(31), _T("%g"), 
				GetLocaleC(), (double)fValue));
		return SetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), forw<S3>(sName), 
				move(sValue), bOverwrite);
	}

	//
	// Color GET-functions
	//

	template <class S>
	COLORREF GetClr(S&& sName) const
	{
		DWORD dw = 0;
		VERIFY(_stscanf(GetStr(forw<S>(sName)), _T("#%6x"), &dw) == 1);
		return RGB(GetBValue(dw), GetGValue(dw), GetRValue(dw));
	}

	template <class S1, class S2>
	COLORREF GetClr(S1&& sCategory, S2&& sName) const
	{
		DWORD dw = 0;
		VERIFY(_stscanf(GetStr(forw<S1>(sCategory), forw<S2>(sName)), _T("#%6x"), &dw) == 1);
		return RGB(GetBValue(dw), GetGValue(dw), GetRValue(dw));
	}

	template <class S1, class S2, class S3>
	COLORREF GetClr(S1&& sCategory, S2&& sSubCategory, S3&& sName) const
	{
		DWORD dw = 0;
		VERIFY(_stscanf(GetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), forw<S3>(sName)), 
				_T("#%6x"), &dw) == 1);
		return RGB(GetBValue(dw), GetGValue(dw), GetRValue(dw));
	}

	//
	// Color SET-functions
	//

	template <class S>
	bool SetClr(S&& sName, COLORREF clrValue, bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf(sValue.GetBuffer(15), _T("#%6x"), clrValue));
		return SetStr(forw<S>(sName), move(sValue), bOverwrite);
	}

	template <class S1, class S2>
	bool SetClr(S1&& sCategory, S2&& sName, COLORREF clrValue, bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf(sValue.GetBuffer(15), _T("#%6x"), clrValue));
		return SetStr(forw<S1>(sCategory), forw<S2>(sName), move(sValue), bOverwrite);
	}

	template <class S1, class S2, class S3>
	bool SetClr(S1&& sCategory, S2&& sSubCategory, S3&& sName, COLORREF clrValue, 
			bool bOverwrite = true)
	{
		RString sValue;
		sValue.ReleaseBuffer(_stprintf(sValue.GetBuffer(15), _T("#%6x"), clrValue));
		return SetStr(forw<S1>(sCategory), forw<S2>(sSubCategory), forw<S3>(sName), 
				move(sValue), bOverwrite);
	}

protected:
	RString m_sFilePath, m_sApplicationID, m_sRootElemName;
	RXMLFile2 m_xmlFile;
};

#endif // __RPREFERENCESMGR2_H__
