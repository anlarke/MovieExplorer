#include "stdafx.h"
#include "MovieExplorer.h"
#include "UpdateThread.h"
#include "ParseFileName.h"
#include "ScrapeIMDb.h"
#include "ScrapeMovieMeter.h"

UINT CALLBACK UpdateThread(void *pParam)
{
	// Save window handle, and get preferences

	HWND hDatabaseWnd = ((UPDATETHREADDATA*)pParam)->hDatabaseWnd;

	RString strTitleServ = GETPREFSTR(_T("InfoService"), _T("Title"));
	RString strYearServ = GETPREFSTR(_T("InfoService"), _T("Year"));
	RString strCountriesServ = GETPREFSTR(_T("InfoService"), _T("Countries"));
	RString strGenresServ = GETPREFSTR(_T("InfoService"), _T("Genres"));
	RString strRuntimeServ = GETPREFSTR(_T("InfoService"), _T("Runtime"));
	RString strStorylineServ = GETPREFSTR(_T("InfoService"), _T("Storyline"));
	RString strDirectorsServ = GETPREFSTR(_T("InfoService"), _T("Directors"));
	RString strWritersServ = GETPREFSTR(_T("InfoService"), _T("Writers"));
	RString strStarsServ = GETPREFSTR(_T("InfoService"), _T("Stars"));
	RString strPosterServ = GETPREFSTR(_T("InfoService"), _T("Poster"));
	RString strRatingServ = GETPREFSTR(_T("InfoService"), _T("Rating"));



	UINT64 nWeeks = (UINT64)GETPREFINT(_T("Database"), _T("MaxInfoAge"));
	if (nWeeks < 2)
		nWeeks = 2;
	UINT64 maxTimeDiff = nWeeks * 7 * 24 * 60 * 60 * 10000000; // 1 second = 10000000 100th-nanoseconds
	UINT64 currentTime = GetSystemTime();
	UINT64 minTime = currentTime - maxTimeDiff;

	RString strCacheDir = CorrectPath(GETPREFSTR(_T("Database"), _T("CacheDirectory")));
	
	// Create thread message queue and signal we're ready

	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	
	((UPDATETHREADDATA*)pParam)->eReady.SetEvent();

	// Determine which services are in use

	RObArray<RString> servicesInUse;
	if (servicesInUse.IndexOf(strTitleServ) == -1) servicesInUse.Add(strTitleServ);
	if (servicesInUse.IndexOf(strYearServ) == -1) servicesInUse.Add(strYearServ);
	if (servicesInUse.IndexOf(strCountriesServ) == -1) servicesInUse.Add(strCountriesServ);
	if (servicesInUse.IndexOf(strGenresServ) == -1) servicesInUse.Add(strGenresServ);
	if (servicesInUse.IndexOf(strRuntimeServ) == -1) servicesInUse.Add(strRuntimeServ);
	if (servicesInUse.IndexOf(strStorylineServ) == -1) servicesInUse.Add(strStorylineServ);
	if (servicesInUse.IndexOf(strDirectorsServ) == -1) servicesInUse.Add(strDirectorsServ);
	if (servicesInUse.IndexOf(strWritersServ) == -1) servicesInUse.Add(strWritersServ);
	if (servicesInUse.IndexOf(strStarsServ) == -1) servicesInUse.Add(strStarsServ);
	if (servicesInUse.IndexOf(strPosterServ) == -1) servicesInUse.Add(strPosterServ);
	if (servicesInUse.IndexOf(strRatingServ) == -1) servicesInUse.Add(strRatingServ);

	foreach (servicesInUse, strServ, i)
		if (strServ.IsEmpty())
			{servicesInUse.RemoveAt(i); break;}

	// Start updating movies

	DBMOVIE mov;
	DBMOVIE *pOrigMov;
	DBINFO info;
	RString strSearchTitle, strSearchYear, strID, strAirDate;
	RXMLFile xmlFile;
	RXMLTag *pInfoTag;
	INT_PTR nUpdatedFromWeb = 0;
	INT_PTR nSeason = -1;
	INT_PTR nEpisode = -1;

	while (SendMessage(hDatabaseWnd, DBM_GETMOVIEUPDATE, (WPARAM)&mov, (LPARAM)&pOrigMov))
	{
		nSeason = -1; nEpisode = -1; strAirDate.Empty();
		ParseFileName(mov.strFileName, strSearchTitle, strSearchYear, nSeason, nEpisode, strAirDate);

		foreach (servicesInUse, strServ)
		{
			ClearInfo(&info);

			// Get the right ID

			if (strServ == _T("imdb.com"))
				strID = mov.strIMDbID;
			else if (strServ == _T("moviemeter.nl"))
				strID = mov.strMovieMeterID;
			else
				ASSERT(false);

			// If there was an error, don't try to update this movie again

			if (strID == _T("unknown") || strID == _T("connError") || strID == _T("scrapeError"))
				continue;

			// Try to update from cache when ID is provided

			if (!strID.IsEmpty())
			{
				if (xmlFile.Read(strCacheDir + _T("\\") + strServ + _T("\\") + strID + _T(".xml")))
				{
					pInfoTag = xmlFile.GetRootTag()->GetChild(_T("MovieInfo"));
					if (pInfoTag)
					{
						ASSERT(pInfoTag->GetChildContent(_T("Timestamp")));
						UINT64 timestamp = StringToNumber64(pInfoTag->GetChildContent(_T("Timestamp")));
						if (timestamp >= minTime)
						{
							TagToInfo(pInfoTag, &info);
							VERIFY(FileToData(strCacheDir + _T("\\") + strServ + _T("\\") + strID + 
									_T(".jpg"), info.posterData));

							for (int i = 0; i < DBI_STAR_NUMBER; i++)
							{
								// Check hash table first then try to load from file cache

								RString strStarName = GetStar(info.strStars, i);
								info.actorImageData[i] = GetImageHash()->GetImage(strStarName);

								if (!info.actorImageData[i] && !strStarName.IsEmpty() && FileExists(strCacheDir + _T("\\") + 
									strServ + _T("\\actors\\") + strStarName + _T(".jpg")))
								{
									RArray<BYTE> arTmp;
									VERIFY(FileToData(strCacheDir + _T("\\") + strServ + _T("\\actors\\")
										+ strStarName + _T(".jpg"), arTmp));
									GetImageHash()->SetImage(strStarName, arTmp);
									info.actorImageData[i] = GetImageHash()->GetImage(strStarName);
								}
							}
							

							info.status = DBI_STATUS_UPDATED;
						}
					}
					else
						ASSERT(false); // invalid cache file
				}
			}

			// Otherwise update from the web

			if (info.status != DBI_STATUS_UPDATED)
			{
				info.strServiceName = strServ;
				info.strSearchTitle = strSearchTitle;
				info.strSearchYear = strSearchYear;
				info.nSeason = nSeason;
				info.nEpisode = nEpisode;
				info.strAirDate = strAirDate;
				info.strID = strID;

				if (strServ == _T("imdb.com"))
					info.status = ScrapeIMDb(&info);
				else if (strServ == _T("moviemeter.nl"))
					info.status = ScrapeMovieMeter(&info);
				else
					ASSERT(false);

				if (PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_NOREMOVE))
					return 0;

				if (info.status == DBI_STATUS_UPDATED)
				{
					strID = info.strID;
					LOG(_T("Succesfully updated ") + info.strTitle + _T(" (") + info.strYear + 
						_T(") from ") + strServ + _T(".\n"));

					++nUpdatedFromWeb;

					info.timestamp = GetSystemTime();
					xmlFile.GetRootTag()->RemoveAllChildren();
					InfoToTag(&info, xmlFile.GetRootTag()->AddChild(_T("MovieInfo")));
					if (!DirectoryExists(strCacheDir))
						CreateDirectory(strCacheDir);
					if (!DirectoryExists(strCacheDir + _T("\\") + strServ))
						CreateDirectory(strCacheDir + _T("\\") + strServ);
					VERIFY(xmlFile.Write(strCacheDir + _T("\\") + strServ + 
							_T("\\") + strID + _T(".xml")));
					VERIFY(DataToFile(info.posterData, strCacheDir + _T("\\") + strServ + 
							_T("\\") + strID + _T(".jpg")));
					
					// save actor images to directory
					
					if (!DirectoryExists(strCacheDir + _T("\\") + strServ + _T("\\actors\\")))
						CreateDirectory(strCacheDir + _T("\\") + strServ + _T("\\actors\\"));

					for (int i = 0; i < DBI_STAR_NUMBER; i++)
					{
						RString strStarName = GetStar(info.strStars, i);
						if (!strStarName.IsEmpty() && info.actorImageData[i] && 
							!FileExists(strCacheDir + _T("\\") + strServ + _T("\\actors\\")
							+ strStarName + _T(".jpg")))
							VERIFY(DataToFile(*info.actorImageData[i], strCacheDir + _T("\\") + strServ + _T("\\actors\\")
							+ strStarName + _T(".jpg")));

					}
				}
				else if (info.status == DBI_STATUS_UNKNOWN)
				{
					strID = _T("unknown");
					LOG(_T("Failed to identify '") + mov.strFileName + _T("' on ") + 
							strServ + _T(".\n"));
				}
				else if (info.status == DBI_STATUS_CONNERROR)
				{
					strID = _T("connError");
					LOG(_T("A connection error occured while identifying ") + mov.strFileName + 
							_T(" on ") + strServ + _T(".\n"));
				}
				else if (info.status == DBI_STATUS_SCRAPEERROR)
				{
					strID = _T("scrapeError");
					LOG(_T("A parsing error occured while identifying ") + mov.strFileName + 
							_T(" on ") + strServ + _T(".\n"));
				}
			}

			// Assign values to movie object

			if (info.status == DBI_STATUS_UPDATED)
			{
				if (strTitleServ == strServ)
					mov.strTitle = info.strTitle;
				if (strYearServ == strServ)
				{
					mov.strYear = info.strYear;
					mov.nYear = StringToNumber(mov.strYear);
				}
				if (strCountriesServ == strServ)
					mov.strCountries = info.strCountries;
				if (strGenresServ == strServ)
					mov.strGenres = info.strGenres;
				if (strRuntimeServ == strServ)
					mov.strRuntime = info.strRuntime;
				if (strStorylineServ == strServ)
					mov.strStoryline = info.strStoryline;
				if (strDirectorsServ == strServ)
					mov.strDirectors = info.strDirectors;
				if (strWritersServ == strServ)
					mov.strWriters = info.strWriters;
				if (strStarsServ == strServ)
					mov.strStars = info.strStars;
				if (strPosterServ == strServ)
					mov.posterData = info.posterData;
				if (strRatingServ == strServ)
				{
					mov.fRating = info.fRating;
					mov.fRatingMax = info.fRatingMax;
					mov.nMetascore = info.nMetascore;
					mov.nVotes = info.nVotes;
				}
				if (mov.fIMDbRating == 0.0f && info.fIMDbRating != 0.0f)
				{
					if (mov.strIMDbID.IsEmpty() || (mov.strIMDbID == info.strIMDbID))
					{
						mov.strIMDbID = info.strIMDbID;
						mov.fIMDbRating = info.fIMDbRating;
						mov.fIMDbRatingMax = info.fIMDbRatingMax;
						mov.nIMDbVotes = info.nIMDbVotes;
					}
				}

				// Set season and episode for "imdb.com"

				if (strServ == _T("imdb.com"))
				{
					mov.nSeason = info.nSeason;
					mov.nEpisode = info.nEpisode;
					mov.strEpisodeName = info.strEpisodeName;
					mov.strAirDate = info.strAirDate;
					for (int i=0; i < DBI_STAR_NUMBER; i++)
						mov.actorImageData[i] = info.actorImageData[i];
				}
			}

			// Save to the right ID

			if (strServ == _T("imdb.com"))
				mov.strIMDbID = strID;
			else if (strServ == _T("moviemeter.nl"))
				mov.strMovieMeterID = strID;
			else
				ASSERT(false);
		}

		mov.bUpdated = true;

		if (PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_NOREMOVE))
			return 0;

		// Deliver updated movie data to main thread

		SendMessage(hDatabaseWnd, DBM_SETMOVIEUPDATE, (WPARAM)&mov, (LPARAM)pOrigMov);

		if (PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_NOREMOVE))
			return 0;
	}

	PostMessage(hDatabaseWnd, DBM_UPDATETHREADEND, GetCurrentThreadId(), nUpdatedFromWeb);
	return 0;
}
