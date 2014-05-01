#include "stdafx.h"
#include "MovieExplorer.h"

void ParseFileName(RString_ strFileName, RString &strTitle, RString &strYear, INT_PTR &nSeason, INT_PTR &nEpisode, RString &strAirDate)
{
	INT_PTR m, n;
	RString strTemp, strSeason, strEpisode;
	RString strYearTmp, strMonthTmp, strDayTmp;

	strTitle = strFileName;
	strYear.Empty();

	// only keep file name

	n = strTitle.ReverseFind(_T('\\'));
	if (n != -1)
		strTitle = strTitle.Mid(n + 1);
	//strTitle.MakeLower();

	// strip file extension (max 4 chars)

	n = strTitle.ReverseFind(_T('.'));
	if (n != -1 && strTitle.GetLength() - n < 6)
		strTitle = strTitle.Left(n);

	// replace []{} by ()

	strTitle.Replace(_T('['), _T('('));
	strTitle.Replace(_T(']'), _T(')'));
	strTitle.Replace(_T('{'), _T('('));
	strTitle.Replace(_T('}'), _T(')'));

	/*
	// replace anything not ,()0-9a-zA-Z by a space

	for (int i = 0; i < strTitle.GetLength(); i++)
	if (!(strTitle[i] == _T(',')) &&
	!(strTitle[i] == _T('\'')) &&
	!(strTitle[i] == _T('(')) &&
	!(strTitle[i] == _T(')')) &&
	!(strTitle[i] >= _T('0') && strTitle[i] <= _T('9')) &&
	!(strTitle[i] >= _T('A') && strTitle[i] <= _T('Z')) &&
	!(strTitle[i] >= _T('a') && strTitle[i] <= _T('z')))
	*((LPTSTR)(LPCTSTR)strTitle + i) = _T(' ');
	*/

	// remove urls

	RString strUrl;
	if (GetFirstMatch(strTitle, _T("([wW][wW][wW]\\.[^\\.]*?\\.[cC][oO][mM])"), &strUrl, NULL))
	{
		m = strTitle.Find(strUrl, 0);
		if (m >= 0)
			strTitle = strTitle.Left(m) + strTitle.Right(strTitle.GetLength() - (m + strUrl.GetLength()));
	}

	// replace ._ -by a space

	strTitle.Replace(_T('.'), _T(' '));
	strTitle.Replace(_T('_'), _T(' '));
	strTitle.Replace(_T('-'), _T(' '));

	// remove redundant space

	while (strTitle.Replace(_T("  "), _T(" ")));
	strTitle.Trim();

	// find year between (), strip anything following it

	for (n = 0; (n = strTitle.Find(_T('('), n)) != -1; n++)
	{
		if (n + 5 < strTitle.GetLength() && strTitle[n + 5] == _T(')'))
		{
			strTemp = strTitle.Mid(n + 1, 4);
			if (StringToNumber(strTemp) > 1900)
			{
				strYear = strTemp;
				strTitle = strTitle.Left(n);
				break;
			}
		}
	}

	// remove anything between ()

	for (m = 0, n = 0; (m = strTitle.Find(_T('('), n)) != -1 &&
		(n = strTitle.Find(_T(')'), m)) != -1; m = 0, n = 0)
		strTitle = strTitle.Left(m) + strTitle.Mid(n + 1);

	// for TV shows. Find the season and episode and remove everything following it

	if (GetFirstMatch(strTitle, _T("([Ss]\\d?\\d[Ee]\\d?\\d)"), &strTemp, NULL))
	{
		m = strTitle.Find(strTemp, 0);
		if (GetFirstMatch(strTemp, _T("[Ss](\\d?\\d)[Ee](\\d?\\d)"), &strSeason, &strEpisode, NULL))
		{
			nSeason = StringToNumber(strSeason);
			nEpisode = StringToNumber(strEpisode);
		}

		if (m >= 0)
			strTitle = strTitle.Left(m);
	}

	// for TV shows by date and parse date aired

	if (GetFirstMatch(strTitle, _T("(\\d\\d\\d\\d) (\\d?\\d) (\\d?\\d)"), &strYearTmp, &strMonthTmp, &strDayTmp, NULL))
	{
		const RString Month[12] = { _T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"),
			_T("May"), _T("Jun"), _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

		strAirDate = strDayTmp + _T(" ") + Month[StringToNumber(strMonthTmp) - 1] + _T(". ") + strYearTmp;
	}

	// find year not in (), but not as first word, strip anything following it

	strTitle.Replace(_T('('), _T(' '));
	strTitle.Replace(_T(')'), _T(' '));
	while (strTitle.Replace(_T("  "), _T(" ")));
	strTitle.Trim();

	if (strYear.IsEmpty())
	{
		m = 0;
		while (m < strTitle.GetLength())
		{
			n = strTitle.Find(_T(' '), m);

			if (n == -1)
				n = strTitle.GetLength();

			if (m == n)
			{
				m = n + 1; continue;
			}

			if (n - m == 4)
			{
				strTemp = strTitle.Mid(m, n - m);
				if (StringToNumber(strTemp) > 1900 && m > 0)
				{
					strYear = strTemp;
					strTitle = strTitle.Left(m);
					break;
				}
			}

			m = n + 1;
		}
	}

	// strip 'season[s] \\d' and anything following it
	RString strSeasons;
	if (GetFirstMatch(strTitle, _T("([Ss]eason[s]? ?\\d?\\d(?: ?- ?\\d?\\d)?)"), &strSeasons, NULL))
	{
		m = strTitle.Find(strSeasons, 0);
		if (m >= 0)
			strTitle = strTitle.Left(m);
	}

	//Remove episode numbers of the form XXofYY - TODO: process to return episode number

	RString strOf;
	if (GetFirstMatch(strTitle, _T("(\\d?\\d[oO][fF]\\d?\\d)"), &strOf, NULL))
	{
		m = strTitle.Find(strOf, 0);
		if (m > 0)
			strTitle = strTitle.Left(m);
	}

	// strip common movie descriptors and everything after

	RString strDescriptors[] = { _T("webrip"), _T("dvdrip"), _T("dvdscr"), _T("xvid"), _T("bdrip"),
		_T("brrip"), _T("hdtv"), _T("pdtv"), _T("box set"), _T("box-set"), _T("x264") };
	foreach(strDescriptors, strD)
	{
		m = strTitle.FindNoCase(strD, 0);
		if (m > 0)
			strTitle = strTitle.Left(m);
	}

	// strip common tv network prefixes

	RString strNetworks[] = { _T("Discovery Channel"), _T("Discovery Ch"), _T("National Geographic"), _T("NG"), _T("Ch4"), _T("HBO") };
	foreach(strNetworks, strN)
	{
		m = strTitle.Find(strN, 0);
		if (m == 0)
			strTitle = strTitle.Right(strTitle.GetLength()-strN.GetLength());
	}

	while (strTitle.Replace(_T("  "), _T(" ")));
	strTitle.Trim();

	// place some literals in front

	if (strTitle.GetLength() > 5 && strTitle.Right(5) == _T(", the"))
		strTitle = _T("the ") + strTitle.Left(strTitle.GetLength() - 5);
	if (strTitle.GetLength() > 5 && strTitle.Right(5) == _T(", The"))
		strTitle = _T("The ") + strTitle.Left(strTitle.GetLength() - 5);
	if (strTitle.GetLength() > 3 && strTitle.Right(3) == _T(", a"))
		strTitle = _T("a ") + strTitle.Left(strTitle.GetLength() - 3);
	if (strTitle.GetLength() > 3 && strTitle.Right(3) == _T(", A"))
		strTitle = _T("A ") + strTitle.Left(strTitle.GetLength() - 3);
	
	strTitle.Replace(_T(','), _T(' '));
	while (strTitle.Replace(_T("  "), _T(" ")));
	strTitle.Trim();

	//TRACE0(_T("ParseFileName\n  strFileName = ") + strFileName + _T("\n  strTitle = ") +
	//		strTitle + _T("\n  strYear = ") + strYear + _T("\n"));
}
