#pragma once

#ifdef _UNICODE
	#undef _tsetlocale
	#undef _totlower
	#undef _totupper
	#undef _tcsicmp
	#undef _tcsnicmp
	#define _tsetlocale		_wsetlocale_map
	#define _totlower		towlower_map
	#define _totupper		towupper_map
	#define _tcsistr		wcsistr
	#define _tcsichr		wcsichr
	#define _tcsicmp		_wcsicmp_map
	#define _tcsnicmp		_wcsnicmp_map
#endif

wchar_t* _wsetlocale_map(int category, const wchar_t *locale);
wchar_t towlower_map(wchar_t c);
wchar_t towupper_map(wchar_t c);
const wchar_t* wcsistr(const wchar_t *str, const wchar_t *subStr);
wchar_t* wcsistr(wchar_t *str, const wchar_t *subStr);
const wchar_t* wcsichr(const wchar_t *str, wchar_t c);
wchar_t* wcsichr(wchar_t *str, wchar_t c);
int _wcsicmp_map(const wchar_t *str1, const wchar_t *str2);
int _wcsnicmp_map(const wchar_t *str1, const wchar_t *str2, size_t count);
