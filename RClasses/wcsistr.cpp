#include "stdafx.h"
#include "wcsistr.h"

wchar_t g_mapToLower[256];
wchar_t g_mapToUpper[256];

wchar_t* _wsetlocale_map(int category, const wchar_t *locale)
{
	wchar_t *ret = _wsetlocale(category, locale);

	for (wchar_t c = 0; c < 256; ++c)
	{
		g_mapToLower[c] = towlower(c);
		g_mapToUpper[c] = towupper(c);
	}
	
	return ret;
}

wchar_t towlower_map(wchar_t c)
	{return (c < 256 ? g_mapToLower[c] : towlower(c));}

wchar_t towupper_map(wchar_t c)
	{return (c < 256 ? g_mapToUpper[c] : towupper(c));}

const wchar_t* wcsistr(const wchar_t *str, const wchar_t *subStr)
{
	if (!*subStr)
		return str;

	const wchar_t l = towlower_map(*subStr);
	const wchar_t u = towupper_map(*subStr);
	const wchar_t *s1, *s2;

	for (; *str; ++str)
	{
		if (*str == l || *str == u)
		{
			s1 = str + 1;
			s2 = subStr + 1;
			
			while (*s1 && *s2 && towlower_map(*s1) == towlower_map(*s2))
				++s1, ++s2;
			
			if (!*s2)
				return str;
		}
	}

	return NULL;
}

wchar_t* wcsistr(wchar_t *str, const wchar_t *subStr)
	{return (wchar_t*)wcsistr((const wchar_t*)str, subStr);}

const wchar_t* wcsichr(const wchar_t *str, wchar_t c)
{
	const wchar_t l = towlower_map(c);
	const wchar_t u = towupper_map(c);

	if (l == u)
		return wcschr(str, c);

	while (*str && *str != l && *str != u)
		++str;

	return (*str ? str : NULL);
}

wchar_t* wcsichr(wchar_t *str, wchar_t c)
	{return (wchar_t*)wcsichr((const wchar_t*)str, c);}

int _wcsicmp_map(const wchar_t *str1, const wchar_t *str2)
{
	while (*str1 && towlower_map(*str1) == towlower_map(*str2))
		++str1, ++str2;

	return (int)(towlower_map(*str1) - towlower_map(*str2));
}

int _wcsnicmp_map(const wchar_t *str1, const wchar_t *str2, size_t count)
{
	if (count)
	{
		while (--count && *str1 && towlower_map(*str1) == towlower_map(*str2))
			++str1, ++str2;

		return (int)(towlower_map(*str1) - towlower_map(*str2));
	}
	return 0;
}
