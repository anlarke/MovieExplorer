#ifndef __RSTRING_H__
#define __RSTRING_H__

class RString;
typedef const RString& RString_;

class RString
{
public:
	RString() : m_lpsz(NULL), m_cch(0)
	{
	}

	RString(const TCHAR *lpsz) : m_lpsz(NULL), m_cch(0)
	{
		operator =(lpsz);
	}

	RString(RString_ str) : m_lpsz(NULL), m_cch(0)
	{
		operator =(str);
	}

#if _MSC_VER >= 1600
	RString(RString&& str) : m_lpsz(str.m_lpsz), m_cch(str.m_cch)
	{
		str.m_lpsz = NULL;
		str.m_cch = 0;
	}
#endif

	~RString()
	{
		Empty();
	}

	RString& operator =(const TCHAR *lpsz)
	{
		if (lpsz)
		{
			m_cch = _tcslen(lpsz);
			if (m_cch != 0)
			{
				m_lpsz = (TCHAR*)realloc(m_lpsz, (m_cch + 1) * sizeof(TCHAR));
				ASSERT(m_lpsz);
				memcpy(m_lpsz, lpsz, (m_cch + 1) * sizeof(TCHAR));
			}
			else
				Empty();
		}
		else
			Empty();

		return *this;
	}

	RString& operator =(RString_ str)
	{
		if (&str != this)
		{
			if (str.m_cch != 0)
			{
				m_cch = str.m_cch;
				m_lpsz = (TCHAR*)realloc(m_lpsz, (m_cch + 1) * sizeof(TCHAR));
				ASSERT(m_lpsz);
				memcpy(m_lpsz, str.m_lpsz, (m_cch + 1) * sizeof(TCHAR));
			}
			else
				Empty();
		}
		return *this;
	}

#if _MSC_VER >= 1600
	RString& operator =(RString&& str)
	{
		if (&str != this)
		{
			free(m_lpsz);
			m_lpsz = str.m_lpsz;
			m_cch = str.m_cch;
			str.m_lpsz = NULL;
			str.m_cch = 0;
		}
		return *this;
	}
#endif

	RString& operator +=(const TCHAR *lpszAppend)
	{
		if (lpszAppend)
		{
			INT_PTR cchAppend = _tcslen(lpszAppend);
			if (cchAppend != 0)
			{
				m_lpsz = (TCHAR*)realloc(m_lpsz, (m_cch + cchAppend + 1) * sizeof(TCHAR));
				ASSERT(m_lpsz);
				memcpy(m_lpsz + m_cch, lpszAppend, (cchAppend + 1) * sizeof(TCHAR));
				m_cch += cchAppend;
			}
		}
		return *this;
	}

	RString& operator +=(RString_ strAppend)
	{
		if (strAppend.m_cch != 0)
		{
			m_lpsz = (TCHAR*)realloc(m_lpsz, (m_cch + strAppend.m_cch + 1) * sizeof(TCHAR));
			ASSERT(m_lpsz);
			memcpy(m_lpsz + m_cch, strAppend.m_lpsz, (strAppend.m_cch + 1) * sizeof(TCHAR));
			m_cch += strAppend.m_cch;
		}
		return *this;
	}

	operator const TCHAR*() const
	{
		return (m_lpsz ? m_lpsz : _T(""));//return m_lpsz;
	}

	const TCHAR operator[](INT_PTR nIndex) const
	{
		ASSERT(m_lpsz);
		ASSERT(nIndex < m_cch);
		return m_lpsz[nIndex];
	}

	TCHAR operator[](INT_PTR nIndex)
	{
		ASSERT(m_lpsz);
		ASSERT(nIndex < m_cch);
		return m_lpsz[nIndex];
	}

	INT_PTR GetLength() const
	{
		return m_cch;
	}

	bool IsEmpty() const
	{
		return (m_cch == 0);
	}

	void Empty()
	{
		free(m_lpsz);
		m_lpsz = NULL;
		m_cch = 0;
	}

	TCHAR* GetBuffer(INT_PTR nMinCharCount)
	{
		if (nMinCharCount < 0)
			{ASSERT(false); return NULL;}
		m_cch = nMinCharCount;
		m_lpsz = (TCHAR*)realloc(m_lpsz, (m_cch + 1) * sizeof(TCHAR));
		ASSERT(m_lpsz);
		return m_lpsz;
	}

	void ReleaseBuffer(INT_PTR nNewCharCount = -1)
	{
		ASSERT(m_lpsz);
		if (!m_lpsz)
			return;

		ASSERT(nNewCharCount >= -1);
		if (nNewCharCount < 0)
		{
			m_lpsz[m_cch] = 0;
			nNewCharCount = _tcslen(m_lpsz);
		}

		ASSERT(nNewCharCount <= m_cch);
		if (nNewCharCount > m_cch)
			nNewCharCount = m_cch;

		if (nNewCharCount != 0)
		{
			if (nNewCharCount < m_cch)
			{
				m_cch = nNewCharCount;
				m_lpsz = (TCHAR*)realloc(m_lpsz, (m_cch + 1) * sizeof(TCHAR));
				ASSERT(m_lpsz);
			}
			m_lpsz[m_cch] = 0;
		}
		else
			Empty();
	}

	INT_PTR Find(TCHAR ch, INT_PTR nStart = 0) const
	{
		if (nStart < 0 || nStart >= m_cch)
			{ASSERT(false); return -1;}

		if (!m_lpsz)
			return -1;

		TCHAR *p = _tcschr(m_lpsz + nStart, ch);
		if (!p)
			return -1;

		return (p - m_lpsz);
	}

#ifdef _tcsichr
	INT_PTR FindNoCase(TCHAR ch, INT_PTR nStart = 0) const
	{
		if (nStart < 0 || nStart >= m_cch)
			{ASSERT(false); return -1;}

		if (!m_lpsz)
			return -1;

		TCHAR *p = _tcsichr(m_lpsz + nStart, ch);
		if (!p)
			return -1;

		return (p - m_lpsz);
	}
#endif

	INT_PTR Find(const TCHAR *lpsz, INT_PTR nStart = 0) const
	{
		if (nStart < 0 || nStart > m_cch)
			{ASSERT(false); return -1;}

		if (!m_lpsz)
			return -1;

		TCHAR *p = _tcsstr(m_lpsz + nStart, lpsz);
		if (!p)
			return -1;

		return (p - m_lpsz);
	}

#ifdef _tcsistr
	INT_PTR FindNoCase(const TCHAR *lpsz, INT_PTR nStart = 0) const
	{
		if (nStart < 0 || nStart > m_cch)
			{ASSERT(false); return -1;}

		if (!m_lpsz)
			return -1;

		TCHAR *p = _tcsistr(m_lpsz + nStart, lpsz);
		if (!p)
			return -1;

		return (p - m_lpsz);
	}
#endif

	INT_PTR ReverseFind(TCHAR ch) const
	{
		if (!m_lpsz)
			return -1;

		TCHAR *p = _tcsrchr(m_lpsz, ch);
		if (!p)
			return -1;

		return (p - m_lpsz);
	}

	INT_PTR FindOneOf(const TCHAR *lpszCharSet, INT_PTR nStart = 0) const
	{
		if (nStart < 0 || nStart >= m_cch)
			{ASSERT(false); return -1;}

		if (!m_lpsz)
			return -1;

		TCHAR *p = _tcspbrk(m_lpsz + nStart, lpszCharSet);
		if (!p)
			return -1;

		return (p - m_lpsz);
	}

	RString Mid(INT_PTR nStart, INT_PTR nLength = -1) const
	{
		RString str;

		if (nLength == -1)
			nLength = m_cch - nStart;

		if (nLength == 0)
			return str;

		if (nStart < 0 || nLength < 0 || nStart + nLength > m_cch)
			{ASSERT(false); return str;}

		str.m_cch = nLength;
		str.m_lpsz = (TCHAR*)malloc((nLength + 1) * sizeof(TCHAR));
		memcpy(str.m_lpsz, m_lpsz + nStart, nLength * sizeof(TCHAR));
		*(str.m_lpsz + nLength) = 0;

		return str;
	}

	RString Left(INT_PTR nLength) const
	{
		return Mid(0, nLength);
	}

	RString Right(INT_PTR nLength) const
	{
		return Mid(m_cch - nLength, nLength);
	}

	INT_PTR Replace(TCHAR chOld, TCHAR chNew)
	{
		//ASSERT(chOld != chNew);

		INT_PTR nCount = 0;

		if (m_lpsz)
			for (TCHAR *p = m_lpsz, *pEnd = p + m_cch; p < pEnd; ++p)
				if (*p == chOld)
					{*p = chNew; ++nCount;}
		
		return nCount;
	}

	INT_PTR Replace(const TCHAR *lpszOld, const TCHAR *lpszNew)
	{
		if (!lpszOld)
			{ASSERT(false); return 0;}

		if (!m_lpsz)
			return 0;

		INT_PTR cchOld = _tcslen(lpszOld);
		if (cchOld == 0)
			return 0;

		if (!lpszNew)
			lpszNew = _T("");

		INT_PTR cchNew = _tcslen(lpszNew);

		//ASSERT(_tcscmp(lpszOld, lpszNew) != 0);

		// When both strings contain a single char use the faster char replace

		if (cchOld == 1 && cchNew == 1)
			return Replace(*lpszOld, *lpszNew);

		// Use in-place algorithm when both strings have the same length

		INT_PTR nCount = 0;
		TCHAR *p;

		if (cchOld == cchNew)
		{
			for (p = m_lpsz; p = _tcsstr(p, lpszOld); p += cchOld, ++nCount)
				memcpy(p, lpszNew, cchOld * sizeof(TCHAR));
			return nCount;
		}

		// First determine the number of replacements

		for (p = m_lpsz; p = _tcsstr(p, lpszOld); p += cchOld, ++nCount);

		if (nCount == 0)
			return 0;

		// Create new buffer

		INT_PTR cch = m_cch + (cchNew - cchOld) * nCount;
		if (cch == 0)
			{Empty(); return nCount;}

		TCHAR *lpsz = (TCHAR*)malloc((cch + 1) * sizeof(TCHAR));
		ASSERT(lpsz);

		// Copy to new buffer in blocks whilst replacing strings

		p = m_lpsz;
		TCHAR *lpszBlockStart = m_lpsz;
		TCHAR *lpszNewBlockStart = lpsz;
		INT_PTR cchBlock;

		while (p = _tcsstr(p, lpszOld))
		{
			cchBlock = p - lpszBlockStart;
			memcpy(lpszNewBlockStart, lpszBlockStart, cchBlock * sizeof(TCHAR));
			lpszNewBlockStart += cchBlock;
			memcpy(lpszNewBlockStart, lpszNew, cchNew * sizeof(TCHAR));
			lpszNewBlockStart += cchNew;
			p += cchOld;
			lpszBlockStart = p;
		}

		memcpy(lpszNewBlockStart, lpszBlockStart, 
				(m_lpsz + m_cch - lpszBlockStart) * sizeof(TCHAR));
		*(lpsz + cch) = 0;

		free(m_lpsz);
		m_lpsz = lpsz;
		m_cch = cch;

		return nCount;
	}

	//INT_PTR Replace(const RObArray<RString>& oldStrings, const RObArray<RString>& newStrings)
	//{
	//	if (oldStrings.GetSize() != newStrings.GetSize())
	//		{ASSERT(false); return 0;}
	//
	//	// TODO: Implement efficiently, requires sort of multi-strstr routine
	//
	//	INT_PTR nCount = 0;
	//	for (INT_PTR i = 0; i < oldStrings; ++i)
	//		nCount += Replace(oldStrings[i], newStrings[i]);
	//
	//	return nCount;
	//}

	void MakeLower()
	{
		if (m_lpsz)
			_tcslwr(m_lpsz);
	}

	void MakeUpper()
	{
		if (m_lpsz)
			_tcsupr(m_lpsz);
	}

	INT_PTR TrimLeft(const TCHAR *lpszTargets = _T(" \t\r\n"))
	{
		ASSERT(lpszTargets);
		if (!m_lpsz || !lpszTargets)
			return 0;

		// Determine trim length

		TCHAR *p = m_lpsz, *pEnd = m_lpsz + m_cch;
		for (; p < pEnd && _tcschr(lpszTargets, *p); ++p);
		INT_PTR nTrimLength = p - m_lpsz;

		if (nTrimLength == 0)
			return 0;

		// Copy to new buffer

		INT_PTR cch = m_cch - nTrimLength;
		TCHAR *lpsz = (TCHAR*)malloc((cch + 1) * sizeof(TCHAR));
		memcpy(lpsz, m_lpsz + nTrimLength, (cch + 1) * sizeof(TCHAR));

		// Use the new buffer

		free(m_lpsz);
		m_lpsz = lpsz;
		m_cch = cch;

		return nTrimLength;
	}

	INT_PTR TrimRight(const TCHAR *lpszTargets = _T(" \t\r\n"))
	{
		ASSERT(lpszTargets);
		if (!m_lpsz || !lpszTargets)
			return 0;

		// Determine trim length

		const TCHAR *p = m_lpsz + m_cch - 1;
		for (; p >= m_lpsz && _tcschr(lpszTargets, *p); p--);
		INT_PTR nTrimLength = (m_lpsz + m_cch - 1) - p;

		if (nTrimLength == 0)
			return 0;

		// Shrink the buffer

		m_cch -= nTrimLength;
		m_lpsz = (TCHAR*)realloc(m_lpsz, (m_cch + 1) * sizeof(TCHAR));
		*(m_lpsz + m_cch) = 0;

		return nTrimLength;
	}

	INT_PTR Trim(const TCHAR *lpszTargets = _T(" \t\r\n"))
	{
		return TrimLeft(lpszTargets) + TrimRight(lpszTargets);
	}

	bool Equals(const TCHAR *lpsz) const
	{
		return (_tcscmp((m_lpsz ? m_lpsz : _T("")), (lpsz ? lpsz : _T(""))) == 0);
	}

	bool Equals(RString_ str) const
	{
		return (m_cch == str.m_cch && _tcscmp((m_lpsz ? m_lpsz : _T("")), 
				(str.m_lpsz ? str.m_lpsz : _T(""))) == 0);
	}

	bool EqualsNoCase(const TCHAR *lpsz) const
	{
		return (_tcsicmp((m_lpsz ? m_lpsz : _T("")), (lpsz ? lpsz : _T(""))) == 0);
	}

	bool EqualsNoCase(RString_ str) const
	{
		return (m_cch == str.m_cch && _tcsicmp((m_lpsz ? m_lpsz : _T("")), 
				(str.m_lpsz ? str.m_lpsz : _T(""))) == 0);
	}
	
	bool NotEquals(const TCHAR *lpsz) const
	{
		return (_tcscmp((m_lpsz ? m_lpsz : _T("")), (lpsz ? lpsz : _T(""))) != 0);
	}

	bool NotEquals(RString_ str) const
	{
		return (m_cch != str.m_cch || _tcscmp((m_lpsz ? m_lpsz : _T("")), 
				(str.m_lpsz ? str.m_lpsz : _T(""))) != 0);
	}

protected:
	TCHAR *m_lpsz;
	INT_PTR m_cch;
};

// Operator +

inline RString operator +(RString_ str1, const TCHAR *lpsz2)
	{RString res(str1); res += lpsz2; return res;}

inline RString operator +(const TCHAR *lpsz1, RString_ str2)
	{RString res(lpsz1); res += str2; return res;}

inline RString operator +(RString_ str1, RString_ str2)
	{RString res(str1); res += str2; return res;}

#if _MSC_VER >= 1600
inline RString operator +(RString&& str1, const TCHAR *lpsz2)
	{RString res(move(str1)); res += lpsz2; return res;}

inline RString operator +(RString&& str1, RString_ str2)
	{RString res(move(str1)); res += str2; return res;}
#endif

// Operator ==

inline bool operator ==(RString_ str1, const TCHAR *lpsz2)
	{return str1.Equals(lpsz2);}

inline bool operator ==(const TCHAR *lpsz1, RString_ str2)
	{return str2.Equals(lpsz1);}

inline bool operator ==(RString_ str1, RString_ str2)
	{return str1.Equals(str2);}

// Operator !=

inline bool operator !=(RString_ str1, const TCHAR *lpsz2)
	{return str1.NotEquals(lpsz2);}

inline bool operator !=(const TCHAR *lpsz1, RString_ str2)
	{return str2.NotEquals(lpsz1);}

inline bool operator !=(RString_ str1, RString_ str2)
	{return str1.NotEquals(str2);}

#endif // __RSTRING_H__
