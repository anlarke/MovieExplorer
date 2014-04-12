#ifndef __RTHEMEMGR2_H__
#define __RTHEMEMGR2_H__

#define GETTHEMESTR				GetThemeMgr()->GetStr
#define GETTHEMEINT				GetThemeMgr()->GetInt
#define GETTHEMEBOOL			GetThemeMgr()->GetBool
#define GETTHEMEFLOAT			GetThemeMgr()->GetFloat
#define GETTHEMECOLOR			GetThemeMgr()->GetClr
#define GETTHEMEFONT			GetThemeMgr()->GetFont
#define GETTHEMEALPHA(...)		(BYTE)round(GetThemeMgr()->GetFloat(__VA_ARGS__) * 255.0)

class RThemeMgr2 : public RPreferencesMgr2
{
public:
	bool Open(RString sFilePath, RString sApplicationID = NULL, 
			RString sRootElemName = _T("ThemeFile"))
		{return RPreferencesMgr2::Open(move(sFilePath), move(sApplicationID), 
				move(sRootElemName));}

	bool SaveAs(RString sFilePath, RString sApplicationID = NULL, 
			RString sRootElemName = _T("ThemeFile"))
		{return RPreferencesMgr2::SaveAs(move(sFilePath), move(sApplicationID), 
				move(sRootElemName));}

	bool GetFont(RFont *pFont, const TCHAR *lpszCategory, const TCHAR *lpszName)
	{
		pFont->Destroy();

		// Get list of preferred fonts and font size

		RString strList = GetStr(lpszCategory, lpszName);
		const TCHAR *lpszSize = GetStr(lpszCategory, (RString)lpszName + _T("Size"));
		float fSize = 0.0f;
		if (lpszSize)
			_stscanf_l(lpszSize, _T("%g"), GetLocaleC(), &fSize);
		bool bBold = GetBool(lpszCategory, (RString)lpszName + _T("Bold"));
		bool bItalic = GetBool(lpszCategory, (RString)lpszName + _T("Italic"));
		
		// Dialog font is the backup font

		LOGFONT lf;
		GetObject(GetDialogFont(), sizeof(LOGFONT), &lf);
		lf.lfWeight = (bBold ? FW_BOLD : FW_NORMAL);
		lf.lfItalic = (bItalic ? TRUE : FALSE);

		strList += _T(", ");
		strList += lf.lfFaceName;

		if (fSize != 0.0f)
			lf.lfHeight = -roundf(fSize * GetScale() * 96.0f/72.0f);

		// Loop through list and try to create font
		
		RArray<const TCHAR*> fonts = SplitString(strList, _T(","), true);
		for (INT_PTR i = 0; i < fonts; ++i)
		{
			RString str(fonts[i]);
			str.Trim();
			_tcsncpy(lf.lfFaceName, str, min(str.GetLength(), 31));
			if (pFont->CreateIndirect(&lf))
				break;
		}

		ASSERTRETURN((HFONT)*pFont != NULL);
	}
};

#endif __RTHEMEMGR2_H__
