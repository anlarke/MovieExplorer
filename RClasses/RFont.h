#ifndef __RFONT_H__
#define __RFONT_H__

#define roundf(x)	(x < 0 ? (int)(x - 0.5f) : (int)(x + 0.5f))

class RFont
{
public:
	RFont() : m_hFont(NULL)
	{
	}

	RFont(const TCHAR *lpszFontName, float fPointSize, bool bBold = false, bool bItalic = false, 
				bool bUnderline = false, bool bStrikeOut = false) : m_hFont(NULL)
	{
		Create(lpszFontName, fPointSize, bBold, bItalic, bUnderline, bStrikeOut);
	}

	RFont(const RFont& fnt)
	{
		operator =(fnt);
	}

	~RFont()
	{
		Destroy();
	}

	bool Create(const TCHAR *lpszFontName, float fPointSize, bool bBold = false, bool bItalic = false, 
				bool bUnderline = false, bool bStrikeOut = false)
	{
		Destroy();
		
		if (!FontExists(lpszFontName))
			return false;

		//HWND hDesktopWnd = GetDesktopWindow();
		//HDC hDesktopDC = GetDC(hDesktopWnd);

		//int nHeight = -MulDiv((int)fPointSize, GetDeviceCaps(hDesktopDC, LOGPIXELSY), 72);
		//int nHeight = -roundf((fPointSize * GetDeviceCaps(hDesktopDC, LOGPIXELSY)) / 72.0f);
		int nHeight = -roundf(fPointSize * GetScale() * 96.0f/72.0f);
		m_hFont = CreateFont(nHeight, 0, 0, 0, (bBold ? FW_BOLD : FW_NORMAL), bItalic, bUnderline, bStrikeOut,
							 DEFAULT_CHARSET, 0, 0, DEFAULT_QUALITY, 0, lpszFontName);
		//ReleaseDC(hDesktopWnd, hDesktopDC);

		ASSERT(m_hFont);
		return m_hFont != NULL;
	}

	void Destroy()
	{
		if (m_hFont)
		{
			VERIFY(DeleteObject(m_hFont));
			m_hFont = NULL;
		}
	}

	bool CreateIndirect(const LOGFONT* pLF)
	{
		Destroy();

		if (!FontExists(pLF->lfFaceName))
			return false;

		m_hFont = CreateFontIndirect(pLF);
		ASSERT(m_hFont);
		return m_hFont != NULL;
	}

	bool CreateIndirect(const HFONT hFont)
	{
		Destroy();

		if (!hFont)
			{ASSERT(false); return false;}

		LOGFONT lf;
		if (GetObject(hFont, sizeof(LOGFONT), &lf) != sizeof(LOGFONT))
			{ASSERT(false); return false;}

		return CreateIndirect(&lf);
	}

	RFont& operator =(const RFont& fnt)
	{
		VERIFY(CreateIndirect(fnt));
		return *this;
	}

	operator HFONT() const
	{
		return m_hFont;
	}

	static bool FontExists(const TCHAR *lpszFontName)
	{		
		LOGFONT lf;
		ZeroMemory(&lf, sizeof(LOGFONT));
		_tcsncpy(lf.lfFaceName, lpszFontName, min(_tcslen(lpszFontName), 31));
		HWND hDesktopWnd = GetDesktopWindow();
		HDC hDesktopDC = GetDC(hDesktopWnd);
		bool bExists = false;
		EnumFontFamiliesEx(hDesktopDC, &lf, EnumFontFamExProc, (LPARAM)&bExists, 0);
		ReleaseDC(hDesktopWnd, hDesktopDC);
		return bExists;
	}

protected:
	static int CALLBACK EnumFontFamExProc(const LOGFONT *pLF, const TEXTMETRIC *pTM, DWORD type, LPARAM lParam)
	{
		*((bool*)lParam) = true;
		return FALSE;
	}

	HFONT m_hFont;
};

inline bool FontExists(const TCHAR *lpszFontName)
	{return RFont::FontExists(lpszFontName);}

#endif // __RFONT_H__
