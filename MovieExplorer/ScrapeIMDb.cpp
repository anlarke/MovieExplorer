#include "stdafx.h"
#include "MovieExplorer.h"

inline RString StripTags(RString str)
{
	RString strMatch;
	while (GetFirstMatch(str, _T("(<[^>]*>)"), &strMatch, NULL))
		str.Replace(strMatch, _T(""));
	return str;
}

DWORD ScrapeIMDb(DBINFO *pInfo)
{
	Sleep((200 * rand()) / RAND_MAX);

	bool bUseBingSearch = GETPREFBOOL(_T("IMDbUseBingSeach"));
	bool bUseOriginalTitle = GETPREFBOOL(_T("IMDbUseOriginalTitle"));

	if (!pInfo || pInfo->strSearchTitle.IsEmpty() || pInfo->strServiceName != _T("imdb.com"))
	{
		ASSERT(false); return DBI_STATUS_SCRAPEERROR;
	}

	RRegEx regex, regex2;
	RString str, strSeasonPage, strEpisodePage, strTemp, strTemp2;
	const TCHAR *p, *pEnd;

	// Find movie ID when it is not provided

	if (bUseBingSearch)
	{
		RString strURL = _T("http://www.bing.com/search?q=site%3Aimdb.com+");
		if (pInfo->strID.IsEmpty())
		{
			strURL += URLEncode(pInfo->strSearchTitle);
			if (!pInfo->strSearchYear.IsEmpty())
				strURL += _T("+%28") + pInfo->strSearchYear + _T("%29");
		}
		else
			strURL += pInfo->strID;

		str = FixLineEnds(HTMLEntitiesDecode(URLToString(strURL)));
		if (str.IsEmpty())
			return DBI_STATUS_CONNERROR;

		if (!GetFirstMatch(str, _T("<a href=\"http://www\\.imdb\\.com/title/(tt\\d+?)/\"[^>]+>(.+?)</a>"),
			&pInfo->strID, &strTemp, NULL))
			return DBI_STATUS_UNKNOWN;

		strTemp = StripTags(strTemp);

		if (!GetFirstMatch(strTemp, _T("(.+?) \\(.*?(\\d+).*?\\)"), &pInfo->strTitle, &pInfo->strYear, NULL))
			return DBI_STATUS_SCRAPEERROR;

		str = FixLineEnds(HTMLEntitiesDecode(URLToString(_T("http://www.imdb.com/title/") + pInfo->strID + _T("/"))));
		if (str.IsEmpty())
			return DBI_STATUS_CONNERROR;
	}
	else
	{
		if (pInfo->strID.IsEmpty())
		{
			RString strURL = _T("http://www.imdb.com/find?s=tt&q=") + URLEncode(pInfo->strSearchTitle);
			if (!pInfo->strSearchYear.IsEmpty())
				strURL += _T("+%28") + pInfo->strSearchYear + _T("%29");

			str = FixLineEnds(HTMLEntitiesDecode(URLToString(strURL)));
			if (str.IsEmpty())
				return DBI_STATUS_CONNERROR;

			if (!GetFirstMatch(str, _T("<title>([^<]+)</title>"), &strTemp, NULL))
				return DBI_STATUS_SCRAPEERROR;

			if (strTemp == _T("Find - IMDb")) // received search results
			{
				if (pInfo->strSearchYear.IsEmpty())
				{
					// 1. Try to match exact title
					// 2. Try to match title partly
					// 3. Just take the first one

					if (!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">") + pInfo->strSearchTitle +
						_T("</a> \\("), &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">[^<]*?") + pInfo->strSearchTitle +
						_T("[^<]*?</a> \\("), &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)"), &pInfo->strID, NULL))
						return DBI_STATUS_UNKNOWN;
				}
				else
				{
					// 1. Try to match exact title + year
					// 2. Try to match exact title + (year-1)
					// 3. Try to match exact title + (year+1)
					// 4. Try to match year
					// 5. Try to match exact title
					// 6. Try to match title partly
					// 7. Just take the first one

					// NOTE: don't add closing ) after year because it may have /I or /II after it

					RString strYearMinusOne = NumberToString(StringToNumber(pInfo->strSearchYear) - 1);
					RString strYearPlusOne = NumberToString(StringToNumber(pInfo->strSearchYear) + 1);

					if (!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">") + pInfo->strSearchTitle + _T("</a> \\(") +
						pInfo->strSearchYear, &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">") + pInfo->strSearchTitle + _T("</a> \\(") +
						strYearMinusOne, &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">") + pInfo->strSearchTitle + _T("</a> \\(") +
						strYearPlusOne, &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">[^<]+</a> \\(") +
						pInfo->strSearchYear, &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">") + pInfo->strSearchTitle +
						_T("</a> \\("), &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)/';\">[^<]*?") + pInfo->strSearchTitle +
						_T("[^<]*?</a> \\("), &pInfo->strID, NULL) &&
						!GetFirstMatch(str, _T("href=\"/title/(tt\\d+)"), &pInfo->strID, NULL))
						return DBI_STATUS_UNKNOWN;
				}

				str = FixLineEnds(HTMLEntitiesDecode(URLToString(_T("http://www.imdb.com/title/") + pInfo->strID + _T("/"))));
				if (str.IsEmpty())
					return DBI_STATUS_CONNERROR;
			}
			else // redirected to movie immediately
			{
				if (!GetFirstMatch(str, _T("imdb.com/title/(tt\\d+)"), &pInfo->strID, NULL))
					return DBI_STATUS_SCRAPEERROR;
			}
		}
		else // id is already provided, just go to the right page
		{
			str = FixLineEnds(HTMLEntitiesDecode(URLToString(_T("http://www.imdb.com/title/") + pInfo->strID + _T("/"))));
			if (str.IsEmpty())
				return DBI_STATUS_CONNERROR;
		}
	}

	// If its a TV show get the episodes page for the proper season.
	// Search for new name and id based on season and episode number.
	// If it exists use it. otherwise use the tv shows main name and id.
	if (pInfo->nSeason >= 0)
	{

		// LOG(pInfo->strSearchTitle + _T(": season ") + NumberToString(pInfo->nSeason) +
		//	_T(", episode ") + NumberToString(pInfo->nEpisode) + _T("\n"));
		strSeasonPage = FixLineEnds(HTMLEntitiesDecode(URLToString(_T("http://www.imdb.com/title/") + pInfo->strID 
			+ _T("/episodes?season=") + NumberToString(pInfo->nSeason))));
		if (!strSeasonPage.IsEmpty())
		{
			// LOG(NumberToString(pInfo->nSeason) + _T(": Found Season Page\n"));
			RString strTmpID;
			if (GetFirstMatch(strSeasonPage, _T("<div>S") + NumberToString(pInfo->nSeason) + _T(", Ep") +
				NumberToString(pInfo->nEpisode) + _T("</div>[\\s\\S]*?/title/(tt\\d+)"), &strTmpID, NULL))
			{
				// LOG(_T("Updated to season/episode id:") + strTmpID + _T("\n"));
				strEpisodePage = FixLineEnds(HTMLEntitiesDecode(URLToString(_T("http://www.imdb.com/title/") + strTmpID + _T("/"))));
				if (!strEpisodePage.IsEmpty())
				{
					str = strEpisodePage;
					pInfo->strID = strTmpID;
					//LOG(_T("New strID: " + pInfo->strID + _T("\n")));
				}
			}
		}
	}

	// Get poster

	if (_tcsicmp(GETPREFSTR(_T("InfoService"), _T("Poster")), _T("imdb.com")) == 0)
	{
		if (GetFirstMatch(str, _T("Poster\"[\\s\\S]*?(http://ia\\.media-imdb\\.com/images/M/.+?_V1\\.?_.*?)\\.(.+?)\""),
			&strTemp, &strTemp2, NULL))
		{
			strTemp = strTemp + _T(".") + strTemp2; // take the server's default cropping and resizing
			URLToData(strTemp, pInfo->posterData);
		}
	}

	// Get rating

	if (GetFirstMatch(str, _T("<span itemprop=\"ratingValue\">([^<]+)</span>"), &strTemp, NULL))
		pInfo->fRating = StringToFloat(strTemp);

	pInfo->fRatingMax = 10.0f;

	// Get Metacritic rating

	if (GetFirstMatch(str, _T("Metascore[ \\t]*:[ \\t]*<[^>]+>[ \\t]*(1?\\d?\\d)/100"), &strTemp, NULL))
		pInfo->nMetascore = StringToNumber(strTemp);

	// Get votes

	if (GetFirstMatch(str, _T("<span itemprop=\"ratingCount\">([^<]+)</span>"), &strTemp, NULL))
	{
		strTemp.Replace(_T(","), _T(""));
		pInfo->nVotes = StringToNumber(strTemp);
	}

	// Get title and year

	if (pInfo->strTitle.IsEmpty() && !GetFirstMatch(str, _T("<title>(.+?) \\(\\D*(\\d+)\\D*\\) - IMDb</title>"),
		&pInfo->strTitle, &pInfo->strYear, NULL))
		return DBI_STATUS_SCRAPEERROR;

	// For TV try to get the episode release date and episode title
	if (pInfo->nSeason >= 0 && !GetFirstMatch(str, _T("<title>\".+?\"[ \t]*(.+?)[ \t]*\\(TV Episode \\D*(\\d+)\\D*\\) - IMDb</title>"), 
		&pInfo->strEpisodeName, &pInfo->strYear, NULL))
		return DBI_STATUS_SCRAPEERROR;

	if (bUseOriginalTitle)
		GetFirstMatch(str, _T(">[^<]*?\"([^<]*?)\"[^<]*?<i>\\(original title\\)</i>"), 
				&pInfo->strTitle, NULL);

	if (pInfo->strTitle.GetLength() >= 2 && pInfo->strTitle.Left(2) == _T("A "))
		pInfo->strTitle = pInfo->strTitle.Mid(2) + _T(", A");
	else if (pInfo->strTitle.GetLength() >= 4 && pInfo->strTitle.Left(4) == _T("The "))
		pInfo->strTitle = pInfo->strTitle.Mid(4) + _T(", The");

	// Get runtime

	GetFirstMatch(str, _T("(\\d+ min\\b)"), &pInfo->strRuntime, NULL);

	// Get directors

	if (GetFirstMatch(str, _T("Directors?:.*?</h4>(.*?)</div>"), &strTemp, NULL))
	{
		RRegEx regex(_T("<a.*?name/(nm\\d+).*?><span.*?>(.*?)</span></a>"));
		p = strTemp; pEnd = strTemp + strTemp.GetLength();
		while (p < pEnd && regex.Search(p, pEnd, NULL, &p))
			if (pInfo->strDirectors.Find(regex.GetMatch(2)))
				pInfo->strDirectors += regex.GetMatch(2) + _T("|");
		pInfo->strDirectors.Trim(_T("|"));
	}

	// Get writers

	if (GetFirstMatch(str, _T("Writers?:.*?</h4>(.*?)</div>"), &strTemp, NULL))
	{
		RRegEx regex(_T("<a.*?name/(nm\\d+).*?><span.*?>(.*?)</span></a>"));
		p = strTemp; pEnd = strTemp + strTemp.GetLength();
		while (p < pEnd && regex.Search(p, pEnd, NULL, &p))
			if (pInfo->strWriters.Find(regex.GetMatch(2)) == -1)
				pInfo->strWriters += regex.GetMatch(2) + _T("|");
		pInfo->strWriters.Trim(_T("|"));
	}

	// Get actors

	if (GetFirstMatch(str, _T("\\.  With (.*?)\\w\\w\\w\\."), &pInfo->strStars, NULL))
		pInfo->strStars.Replace(_T(", "), _T("|"));

	// Get storyline

	if (GetFirstMatch(str, _T("<h2>Storyline</h2>.*?<p>(.*?) *(?:<em class=\"nobr\">|</p>)"), &strTemp, NULL))
	{
		VERIFY(regex.Create(_T("<[^>]+>")));
		pInfo->strStoryline = regex.Replace(strTemp, _T(""));
		pInfo->strStoryline = FixLineEnds(pInfo->strStoryline, _T(""));
	}

	// Get genres
	
	if (GetFirstMatch(str, _T("<h4 class=\"inline\">Genres?:</h4>(.*?)</div>"), &strTemp, NULL))
	{
		VERIFY(regex.Create(_T("<a.*?>\\s*(.*?)\\s*</a>")));
		p = strTemp; pEnd = strTemp + strTemp.GetLength();
		while (p < pEnd && regex.Search(p, pEnd, NULL, &p))
			pInfo->strGenres += regex.GetMatch(1) + _T("|");
		pInfo->strGenres.Trim(_T("|"));
	}

	// Get countries
	
	if (GetFirstMatch(str, _T("<h4 class=\"inline\">Countr(?:y|ies):</h4>(.*?)</div>"), &strTemp, NULL))
	{
		VERIFY(regex.Create(_T("<a.*?>(.*?)</a>")));
		p = strTemp; pEnd = strTemp + strTemp.GetLength();
		while (p < pEnd && regex.Search(p, pEnd, NULL, &p))
			pInfo->strCountries += regex.GetMatch(1) + _T("|");
		pInfo->strCountries.Trim(_T("|"));
	}

	return DBI_STATUS_UPDATED;
}
