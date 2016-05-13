#include "stdafx.h"
#include "MovieExplorer.h"

inline RString StripTags(RString str)
{
	RString strMatch;
	while (GetFirstMatch(str, _T("(<[^>]*>)"), &strMatch, NULL))
		str.Replace(strMatch, _T(""));
	return str;
}

DWORD ScrapeMovieMeter(DBINFO *pInfo)
{
	Sleep((200 * rand()) / RAND_MAX);

	if (!pInfo || pInfo->strSearchTitle.IsEmpty() || pInfo->strServiceName != _T("moviemeter.nl"))
		{ASSERT(false); return DBI_STATUS_SCRAPEERROR;}

	pInfo->strTitle = pInfo->strYear = pInfo->strGenres = pInfo->strCountries = pInfo->strContentRating = 
			pInfo->strStoryline = pInfo->strDirectors = pInfo->strWriters = pInfo->strStars = 
			pInfo->strStars = (const TCHAR*)NULL;
	pInfo->fRating = pInfo->fRatingMax = 0.0f;
	pInfo->nVotes = 0;
	pInfo->nRuntime = 0;
	pInfo->nMetascore = -1;
	pInfo->posterData.SetSize(0);
	pInfo->bType = DB_TYPE_UNKNOWN;
	for (int i = 0; i < DBI_STAR_NUMBER; i++)
	{
		pInfo->actorImageData[i] = NULL;
		pInfo->strActorId[i].Empty();
	}
	//pInfo->status = DBI_STATUS_NONE;
	//pInfo->timestamp = 0;

	//
	// Find movie ID when it is not provided
	//

	if (pInfo->strID.IsEmpty())
	{
		RString strURL = _T("http://www.bing.com/search?q=site%3Amoviemeter.nl+") + URLEncode(pInfo->strSearchTitle);
		if (!pInfo->strSearchYear.IsEmpty())
			strURL += _T("+%28") + pInfo->strSearchYear + _T("%29");

		RString str = FixLineEnds(HTMLEntitiesDecode(URLToString(strURL)));
		if (str.IsEmpty())
			return DBI_STATUS_CONNERROR;

		if (!GetFirstMatch(str, _T("<a href=\"http://www\\.moviemeter\\.nl/film/(\\d+?)\"[^>]+>.+?</a>"),
				&pInfo->strID, NULL))
			return DBI_STATUS_UNKNOWN;
	}

	//
	// Retrieve page from MovieMeter
	//

	RString str = FixLineEnds(HTMLEntitiesDecode(URLToString(_T("http://www.moviemeter.nl/film/") + pInfo->strID)));
	if (str.IsEmpty())
		return DBI_STATUS_CONNERROR;

	//
	// Download poster
	//

	RString strPosterURL;
	if (GetFirstMatch(str, _T("(http://www\\.moviemeter\\.nl/images/cover/[^\"]*)"), &strPosterURL, NULL))
	{
		strPosterURL.Replace(_T(".300."), _T(".200.")); // prefer with of 200px
		URLToData(strPosterURL, pInfo->posterData);
	}

	//
	// Extract title and year
	//

	GetFirstMatch(str, _T("<title>(.*?) \\((\\d*)\\)"), 
			&pInfo->strTitle, &pInfo->strYear, NULL);

	if (pInfo->strTitle.GetLength() >= 2 && pInfo->strTitle.Left(2) == _T("A "))
		pInfo->strTitle = pInfo->strTitle.Mid(2) + _T(", A");
	else if (pInfo->strTitle.GetLength() >= 4 && pInfo->strTitle.Left(4) == _T("The "))
		pInfo->strTitle = pInfo->strTitle.Mid(4) + _T(", The");

	//
	// Extract description
	//
	RString strRuntime;
	GetFirstMatch(str, _T(">([^<]*)<br /><span itemprop=\"genre\">(.*?)</span><br />(.*?)<br /></p>")
			_T("<p>geregisseerd door (.*?)<br />met (.*?)<p itemprop=\"description\">(.*?)</p>"), 
			&pInfo->strCountries, &pInfo->strGenres, &strRuntime, &pInfo->strDirectors, 
			&pInfo->strStars, &pInfo->strStoryline, NULL);

	
	pInfo->strCountries = StripTags(pInfo->strCountries);
	pInfo->strCountries.Replace(_T(" / "), _T("|"));

	pInfo->strGenres = StripTags(pInfo->strGenres);
	pInfo->strGenres.Replace(_T(" / "), _T("|"));

	strRuntime = StripTags(strRuntime);
	pInfo->nRuntime = StringToNumber(strRuntime);

	pInfo->strDirectors = StripTags(pInfo->strDirectors);
	pInfo->strDirectors.Replace(_T(", "), _T("|"));
	pInfo->strDirectors.Replace(_T(" en "), _T("|"));

	pInfo->strStars = StripTags(pInfo->strStars);
	pInfo->strStars.Replace(_T(", "), _T("|"));
	pInfo->strStars.Replace(_T(" en "), _T("|"));

	//
	// Extract rating and votes
	//

	RString strRating, strVotes;
	GetFirstMatch(str, _T("<div class=\"film_average\"><span>([^<]*)</span>")
			_T("</div><div class=\"film_votes\">([^<]*) stemmen</div>"), 
			&strRating, &strVotes, NULL);
	strRating.Replace(_T(','), _T('.'));
	pInfo->nVotes = StringToNumber(strVotes);
	pInfo->fRating = StringToFloat(strRating);
	pInfo->fRatingMax = 5.0f;

	//
	// Get IMDb ID and rating
	//

	pInfo->strIMDbID.Empty();
	pInfo->fIMDbRating = 0.0f;
	pInfo->fIMDbRatingMax = 0.0f;
	pInfo->nIMDbVotes = 0;

	if (GetFirstMatch(str, _T("imdb\\.com/title/(tt\\d+)/\" >IMDb \\((.+?) / (\\d+)\\)"),
			&pInfo->strIMDbID, &strRating, &strVotes, NULL))
	{
		strRating.Replace(_T(','), _T('.'));
		pInfo->fIMDbRating = StringToFloat(strRating);
		pInfo->fIMDbRatingMax = 10.0f;
		pInfo->nIMDbVotes = StringToNumber(strVotes);
	}

	return DBI_STATUS_UPDATED;
}
