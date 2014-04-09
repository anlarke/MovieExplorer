#pragma once

inline RString URLEncode(RString_ str)
{
	static const TCHAR *Dec2Hex = _T("0123456789ABCDEF");
	const RArray<BYTE> data = StringToData(str, CHARSET_UTF8);
	RString strRes;
	TCHAR *pStart = strRes.GetBuffer(str.GetLength() * 3);
	TCHAR *pEnd = pStart;

	for (INT_PTR i = 0; i < data.GetSize(); i++)
	{
		if ((data[i] >= _T('a') && data[i] <= _T('z')) ||
			(data[i] >= _T('A') && data[i] <= _T('Z')) ||
			(data[i] >= _T('0') && data[i] <= _T('9')) ||
			data[i] == _T('-') || 
			data[i] == _T('_') || 
			data[i] == _T('.') || 
			data[i] == _T('~'))
			*pEnd++ = data[i];
		else if (data[i] == _T(' '))
			*pEnd++ = _T('+');
		else
		{
			*pEnd++ = _T('%');
			*pEnd++ = Dec2Hex[data[i] >> 4];
			*pEnd++ = Dec2Hex[data[i] & 0xF];
		}
	}

	strRes.ReleaseBuffer(pEnd - pStart);
	return strRes;
}

inline RString URLDecode(RString_ str)
{
	static BYTE Hex2Dec[256] = {1};
	if (Hex2Dec[0] == 1)
	{
		memset(Hex2Dec, 0, 256);
		Hex2Dec[48] = 0x0; Hex2Dec[49] = 0x1; Hex2Dec[50] = 0x2; Hex2Dec[51] = 0x3; 
		Hex2Dec[52] = 0x4; Hex2Dec[53] = 0x5; Hex2Dec[54] = 0x6; Hex2Dec[55] = 0x7; 
		Hex2Dec[56] = 0x8; Hex2Dec[57] = 0x9; Hex2Dec[65] = 0xA; Hex2Dec[66] = 0xB; 
		Hex2Dec[67] = 0xC; Hex2Dec[68] = 0xD; Hex2Dec[69] = 0xE; Hex2Dec[70] = 0xF; 
		Hex2Dec[97] = 0xA; Hex2Dec[98] = 0xB; Hex2Dec[99] = 0xC; Hex2Dec[100] = 0xD; 
		Hex2Dec[101] = 0xE; Hex2Dec[102] = 0xF;
	}

	RArray<BYTE> data(str.GetLength());
	BYTE *pStart = data.GetData();
	BYTE *pEnd = pStart;

	for (INT_PTR i = 0; i < str.GetLength(); i++)
	{
		if (str[i] == _T('%'))
		{
			if (i + 2 >= str.GetLength())
				{ASSERT(false); break;}
			*pEnd++ = (Hex2Dec[str[i+1]] << 4) + Hex2Dec[str[i+2]];
			i += 2;
		}
		else if (str[i] == _T('+'))
			*pEnd++ = _T(' ');
		else
			*pEnd++ = (BYTE)str[i];
	}

	data.SetSize(pEnd - pStart);
	return DataToString(data, CHARSET_UTF8);
}

inline RString HTMLEntitiesEncode(RString str)
{
	str.Replace(_T("&"), _T("&amp;"));
	str.Replace(_T("<"), _T("&lt;"));
	str.Replace(_T(">"), _T("&gt;"));
	str.Replace(_T("\""), _T("&quot;"));
	str.Replace(_T("'"), _T("&apos;"));
	return str;
}

inline RString HTMLEntitiesDecode(RString str)
{
	INT_PTR m, n;
	for (m = 0; (m = str.Find(_T("&#"), m)) != -1; m++)
	{
		n = str.Find(_T(";"), m);
		if (n == -1)
			continue;

		if (n-m < 3)
			continue;

		if (str[m+2] == _T('x') || str[m+2] == _T('X')) // &#x0; up to &#xFFFF;
		{
			if (m-n > 7)
				continue;

			WCHAR code = 0;
			if (_stscanf(str.Mid(m+3, n-m-3), _T("%X"), &code) != 1)
				continue;

			WCHAR ws[2] = {code, 0};
			str.Replace(str.Mid(m, n-m+1), ws);
		}
		else // &#0; up to &#65535;
		{
			if (m-n > 7)
				continue;

			WCHAR code = (UINT)StringToNumber(str.Mid(m+2, n-m-2));
			WCHAR ws[2] = {code, 0};
			str.Replace(str.Mid(m, n-m+1), ws);
		}
	}

	str.Replace(_T("&nbsp;"), _T(" "));
	str.Replace(_T("&quot;"), _T("\""));
	str.Replace(_T("&apos;"), _T("'"));
	str.Replace(_T("&amp;"), _T("&"));
	str.Replace(_T("&lt;"), _T("<"));
	str.Replace(_T("&gt;"), _T(">"));
	str.Replace(_T("&cent;"), _T("¢"));
	str.Replace(_T("&pound;"), _T("£"));
	str.Replace(_T("&yen;"), _T("¥"));
	str.Replace(_T("&euro;"), _T("€"));
	str.Replace(_T("&sect;"), _T("§"));
	str.Replace(_T("&copy;"), _T("©"));
	str.Replace(_T("&reg;"), _T("®"));
	str.Replace(_T("&trade;"), _T("™"));

	str.Replace(_T("&aacute;"), _T("á"));
	str.Replace(_T("&Aacute;"), _T("Á"));
	str.Replace(_T("&agrave;"), _T("à"));
	str.Replace(_T("&Agrave;"), _T("À"));
	str.Replace(_T("&eacute;"), _T("é"));
	str.Replace(_T("&Eacute;"), _T("É"));
	str.Replace(_T("&oacute;"), _T("ó"));
	str.Replace(_T("&Oacute;"), _T("Ó"));

	// TODO: add much more like these...
	// http://www.w3schools.com/tags/ref_entities.asp

	return str;
}
