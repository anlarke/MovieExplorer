#include "stdafx.h"
#include "MovieExplorer.h"
#include "Database.h"
#include "UpdateThread.h"

#define DB_THREAD_COUNT		4
#define DB_THREAD_WAIT		10000

////////////////////////////////////////////////////////////////////////////////////////////////////
// General functions

void ClearInfo(DBINFO *pInfo)
{
	ASSERT(pInfo);
	pInfo->fRating = 0.0f;
	pInfo->fRatingMax = 0.0f;
	pInfo->fIMDbRating = 0.0f;
	pInfo->fIMDbRatingMax = 0.0f;
	pInfo->nMetascore = -1;
	pInfo->nVotes = 0;
	pInfo->nIMDbVotes = 0;
	pInfo->posterData.SetSize(0);
	pInfo->status = DBI_STATUS_NONE;
	pInfo->strCountries.Empty();
	pInfo->strDirectors.Empty();
	pInfo->strFileName.Empty();
	pInfo->strGenres.Empty();
	pInfo->strID.Empty();
	pInfo->strIMDbID.Empty();
	pInfo->strRuntime.Empty();
	pInfo->strSearchTitle.Empty();
	pInfo->strSearchYear.Empty();
	pInfo->strServiceName.Empty();
	pInfo->strStars.Empty();
	pInfo->strStoryline.Empty();
	pInfo->strTitle.Empty();
	pInfo->strWriters.Empty();
	pInfo->strYear.Empty();
	pInfo->timestamp = 0;
}

void ClearMovie(DBMOVIE *pMovie)
{
	ASSERT(pMovie);
	pMovie->bHide = false;
	pMovie->bSeen = false;
	pMovie->bUpdated = false;
	pMovie->fileSize = 0;
	pMovie->fileTime = 0;
	pMovie->fIMDbRating = 0.0f;
	pMovie->fIMDbRatingMax = 0.0f;
	pMovie->fRating = 0.0f;
	pMovie->fRatingMax = 0.0f;
	pMovie->nIMDbVotes = 0;
	pMovie->nMetascore = -1;
	pMovie->nVotes = 0;
	pMovie->nYear = 0;
	pMovie->pDirectory = NULL;
	pMovie->posterData.SetSize(0);
	pMovie->strCountries.Empty();
	pMovie->strDirectors.Empty();
	pMovie->strFileName.Empty();
	pMovie->strGenres.Empty();
	pMovie->strIMDbID.Empty();
	pMovie->strMovieMeterID.Empty();
	pMovie->strRuntime.Empty();
	pMovie->strStars.Empty();
	pMovie->strStoryline.Empty();
	pMovie->strTitle.Empty();
	pMovie->strWriters.Empty();
	pMovie->strYear.Empty();
}

void TagToInfo(RXMLTag *pTag, DBINFO *pInfo)
{
	ASSERT(pTag);
	ASSERT(pInfo);
	pInfo->strID = pTag->GetChildContent(_T("ID"));
	pInfo->strTitle = pTag->GetChildContent(_T("Title"));
	pInfo->strYear = pTag->GetChildContent(_T("Year"));
	pInfo->strGenres = pTag->GetChildContent(_T("Genres"));
	pInfo->strCountries = pTag->GetChildContent(_T("Countries"));
	pInfo->strRuntime = pTag->GetChildContent(_T("Runtime"));
	pInfo->strStoryline = pTag->GetChildContent(_T("Storyline"));
	pInfo->strDirectors = pTag->GetChildContent(_T("Directors"));
	pInfo->strWriters = pTag->GetChildContent(_T("Writers"));
	pInfo->strStars = pTag->GetChildContent(_T("Stars"));
	pInfo->fRating = StringToFloat(pTag->GetChildContent(_T("Rating")));
	pInfo->fRatingMax = StringToFloat(pTag->GetChildContent(_T("RatingMax")));
	pInfo->nVotes = StringToNumber(pTag->GetChildContent(_T("Votes")));
	pInfo->nMetascore = StringToNumber(pTag->GetChildContent(_T("Metascore")));
	if (pTag->GetChild(_T("IMDbID")))
	{
		pInfo->strIMDbID = pTag->GetChildContent(_T("IMDbID"));
		pInfo->fIMDbRating = StringToFloat(pTag->GetChildContent(_T("IMDbRating")));
		pInfo->fIMDbRatingMax = StringToFloat(pTag->GetChildContent(_T("IMDbRatingMax")));
		pInfo->nIMDbVotes = StringToNumber(pTag->GetChildContent(_T("IMDbVotes")));
	}
	pInfo->timestamp = (UINT64)StringToNumber64(pTag->GetChildContent(_T("Timestamp")));
}

void InfoToTag(DBINFO *pInfo, RXMLTag *pTag)
{
	ASSERT(pInfo);
	ASSERT(pTag);
	pTag->AddChild(_T("ID"))->SetContent(pInfo->strID);
	pTag->AddChild(_T("Title"))->SetContent(pInfo->strTitle);
	pTag->AddChild(_T("Year"))->SetContent(pInfo->strYear);
	pTag->AddChild(_T("Genres"))->SetContent(pInfo->strGenres);
	pTag->AddChild(_T("Countries"))->SetContent(pInfo->strCountries);
	pTag->AddChild(_T("Runtime"))->SetContent(pInfo->strRuntime);
	pTag->AddChild(_T("Storyline"))->SetContent(pInfo->strStoryline);
	pTag->AddChild(_T("Directors"))->SetContent(pInfo->strDirectors);
	pTag->AddChild(_T("Writers"))->SetContent(pInfo->strWriters);
	pTag->AddChild(_T("Stars"))->SetContent(pInfo->strStars);
	pTag->AddChild(_T("Rating"))->SetContent(FloatToString(pInfo->fRating));
	pTag->AddChild(_T("RatingMax"))->SetContent(FloatToString(pInfo->fRatingMax));
	pTag->AddChild(_T("Votes"))->SetContent(NumberToString(pInfo->nVotes));
	pTag->AddChild(_T("Metascore"))->SetContent(NumberToString(pInfo->nMetascore));
	if (!pInfo->strIMDbID.IsEmpty())
	{
		pTag->AddChild(_T("IMDbID"))->SetContent(pInfo->strIMDbID);
		pTag->AddChild(_T("IMDbRating"))->SetContent(FloatToString(pInfo->fIMDbRating));
		pTag->AddChild(_T("IMDbRatingMax"))->SetContent(FloatToString(pInfo->fIMDbRatingMax));
		pTag->AddChild(_T("IMDbVotes"))->SetContent(NumberToString(pInfo->nIMDbVotes));
	}
	pTag->AddChild(_T("Timestamp"))->SetContent(NumberToString((INT64)pInfo->timestamp));
}

bool GetFirstMatch(RString_ strTarget, RString_ strPattern, RString *pStr1, ...)
{
	RArray<RString*> strings;
	strings.Add(pStr1);
	va_list args;
	va_start(args, pStr1);
	while (RString *p = va_arg(args, RString*))
		strings.Add(p);
	va_end(args);

	RRegEx regex;
	if (regex.Create(strPattern) && regex.Search(strTarget))
	{
		ASSERT(strings == regex.GetMatchCount() - 1);
		for (INT_PTR i = 1; i-1 < strings && regex.GetMatch(i, strings[i-1]); ++i);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// CDatabase implementation

CDatabase::CDatabase()
{
	VERIFY(Create<CDatabase>(NULL, WS_OVERLAPPEDWINDOW));
}

CDatabase::~CDatabase()
{
	Unload();
}

bool CDatabase::Load(RString_ strFilePath)
{
	m_strFilePath.Empty();

	LOG(_T("\nLoading database from '") + RelativePath(strFilePath) + _T("'...\n"));
	STATUS(GETSTR(IDS_LOADINGDATABASE) + _T("..."));

	RXMLFile dbFile;
	if (!dbFile.Read(strFilePath))
		{LOG(_T("Failed to open XML-file (invalid or might not exist), loading aborted.\n")); 
				return false;}

	RXMLTag *pMainTag = dbFile.GetRootTag()->GetChild(_T("DatabaseFile"));
	if (!pMainTag)
		{LOG(_T("The 'DatabaseFile' element is missing, loading aborted.\n")); 
				ASSERTRETURN(false);}

	RString strAppID = pMainTag->GetProperty(_T("applicationID"));
	if (strAppID != APPLICATION_ID &&
			strAppID != _T("MOVIEEXPLORER080B"))
		{LOG(_T("Invalid application ID, loading aborted.\n")); 
				ASSERTRETURN(false);}

	INT_PTR nMovies = 0;
	RXMLTag *pCatTag, *pDirTag, *pFileTag;
	const TCHAR *lpsz;
	DBCATEGORY *pCat;
	DBDIRECTORY *pDir;
	DBMOVIE *pMov;
	RString strComputerName;

	for (INT_PTR i = 0; i < pMainTag->GetChildCount(); ++i)
	{
		pCatTag = pMainTag->GetChild(i);
		lpsz = pCatTag->GetProperty(_T("name"));
		 
		// Check if element name is correct and has the name attribute

		if (_tcsicmp(pCatTag->GetName(), _T("Category")) != 0 || !lpsz)
			{LOG(_T("Skipped invalid element '") + pCatTag->GetName() + _T("'.\n")); 
					ASSERT(false); continue;}

		// Check if category does not already exist
		
		foreach (m_categories, cat)
			if (_tcsicmp(cat.strName, lpsz) == 0)
				{LOG(_T("The category '") + lpsz + _T("' already exists, skipped it.\n")); 
						ASSERT(false); continue;}

		// Add category

		pCat = m_categories.AddNew();
		pCat->strName = lpsz;

		// Now add its directories

		for (INT_PTR i = 0; i < pCatTag->GetChildCount(); ++i)
		{
			pDirTag = pCatTag->GetChild(i);
			lpsz = pDirTag->GetProperty(_T("path"));

			// Check if element name is correct and has the path attribute

			if (_tcsicmp(pDirTag->GetName(), _T("Directory")) != 0 || !lpsz)
				{LOG(_T("Skipped invalid element '") + pDirTag->GetName() + _T("'.\n")); 
						ASSERT(false); continue;}

			// Check if path does not already exist in this category

			strComputerName = pDirTag->GetProperty(_T("computerName"));
			foreach (pCat->directories, dir)
				if (_tcsicmp(dir.strComputerName, strComputerName) == 0 &&
						_tcsicmp(CorrectPath(dir.strPath, true), CorrectPath(lpsz, true)) == 0)
					{LOG(_T("The path '") + lpsz + _T("' already exists, skipped it.\n")); 
							ASSERT(false); continue;}

			// Add directory/list

			pDir = pCat->directories.AddNew();
			pDir->pCategory = NULL;
			pDir->strPath = lpsz;
			pDir->strComputerName = strComputerName;

			// Now add its files/entries

			for (INT_PTR i = 0; i < pDirTag->GetChildCount(); ++i)
			{
				pFileTag = pDirTag->GetChild(i);
				lpsz = pFileTag->GetProperty(_T("name"));

				// Check if element name is correct and has the name attribute

				if (_tcsicmp(pFileTag->GetName(), _T("File")) != 0 || !lpsz)
					{LOG(_T("Skipped invalid element '") + pFileTag->GetName() + _T("'.\n"));
							ASSERT(false); continue;}

				// Check if movie does not already exist in this directory

				foreach (pDir->movies, mov)
					if (_tcsicmp(mov.strFileName, lpsz) == 0)
						{LOG(_T("The file '") + lpsz + _T("' already exists, skipped it.\n")); 
								ASSERT(false); continue;}

				// Add movie

				pMov = pDir->movies.AddNew();
				ClearMovie(pMov);
				pMov->strFileName = lpsz;
				pMov->fileSize = StringToNumber64(pFileTag->GetProperty(_T("size")));
				pMov->fileTime = StringToNumber64(pFileTag->GetProperty(_T("time")));
				pMov->bHide = ((RString)pFileTag->GetProperty(_T("hide")) == _T("true"));
				pMov->bSeen = ((RString)pFileTag->GetProperty(_T("seen")) == _T("true"));
				pMov->strIMDbID = pFileTag->GetProperty(_T("imdb.com"));
				pMov->strMovieMeterID = pFileTag->GetProperty(_T("moviemeter.nl"));

				// Determine if an update is needed

				if ((pMov->strIMDbID == _T("unknown") || pMov->strIMDbID == _T("connError") || 
						pMov->strIMDbID == _T("scrapeError")) && 
						(pMov->strMovieMeterID == _T("unknown") || 
						pMov->strMovieMeterID == _T("connError") || 
						pMov->strMovieMeterID == _T("scrapeError")))
					pMov->bUpdated = true;
				else
					pMov->bUpdated = false;

				++nMovies;
			}
		}
	}

	// Set pointers (couldn't do this immediately because when resizing the array the address
	// of the whole array is likely to change)

	foreach (m_categories, cat)
	{
		foreach (cat.directories, dir)
		{
			dir.pCategory = &cat;
			foreach (dir.movies, mov)
				mov.pDirectory = &dir;
		}
	}

	// Loading successful, finalize

	m_strFilePath = strFilePath;

	OnPrefChanged();

	LOG(_T("Done, ") + NumberToString(nMovies) + _T(" movies added.\n"));
	STATUS(GETSTR(IDS_READY));

	Filter();
	SyncAndUpdate();

	return true;
}

void CDatabase::Unload()
{
	CancelUpdate();
	m_categories.SetSize(0);
	Filter();
	m_strFilePath.Empty();
}

bool CDatabase::Save()
{
	if (m_strFilePath.IsEmpty())
		ASSERTRETURN(false);

	RXMLFile dbFile;
	RXMLTag *pMainTag, *pCatTag, *pDirTag, *pFileTag;

	pMainTag = dbFile.GetRootTag()->AddChild(_T("DatabaseFile"));
	pMainTag->SetProperty(_T("applicationID"), APPLICATION_ID);

	foreach (m_categories, cat)
	{
		pCatTag = pMainTag->AddChild(_T("Category"));
		pCatTag->SetProperty(_T("name"), cat.strName);

		foreach (cat.directories, dir)
		{
			pDirTag = pCatTag->AddChild(_T("Directory"));
			pDirTag->SetProperty(_T("path"), dir.strPath);
			if (!dir.strComputerName.IsEmpty())
				pDirTag->SetProperty(_T("computerName"), dir.strComputerName);

			foreach (dir.movies, mov)
			{
				pFileTag = pDirTag->AddChild(_T("File"));
				pFileTag->SetProperty(_T("name"), mov.strFileName);
				if (mov.fileSize != 0)
					pFileTag->SetProperty(_T("size"), NumberToString((INT64)mov.fileSize));
				if (mov.fileTime != 0)
					pFileTag->SetProperty(_T("time"), NumberToString((INT64)mov.fileTime));
				if (mov.bSeen)
					pFileTag->SetProperty(_T("seen"), _T("true"));
				if (mov.bHide)
					pFileTag->SetProperty(_T("hide"), _T("true"));
				if (!mov.strIMDbID.IsEmpty())
					pFileTag->SetProperty(_T("imdb.com"), mov.strIMDbID);
				if (!mov.strMovieMeterID.IsEmpty())
					pFileTag->SetProperty(_T("moviemeter.nl"), mov.strMovieMeterID);
			}
		}
	}

	if (!dbFile.Write(m_strFilePath))
		ASSERTRETURN(false);

	return true;
}

void CDatabase::SyncAndUpdate()
{
	LOG(_T("\nSynchronizing database with files on disk...\n"));

	// Determine total number of directories/lists

	INT_PTR nTotalDirs = 0;
	foreach (m_categories, cat)
		nTotalDirs += cat.directories.GetSize();

	// Create easy to search list of extensions

	RString strIndexExtensions = GETPREFSTR(_T("Database"), _T("IndexExtensions"));
	strIndexExtensions = _T("|") + strIndexExtensions + _T("|");

	// Sync with files on disk

	INT_PTR nDir = 0, nRemoved = 0, nAdded = 0;
	RString strPath;
	bool bIndexDirs = GETPREFBOOL(_T("Database"), _T("IndexDirectories"));
	DBMOVIE *pMov;
	bool bFileExists, bDirExists;

	foreach (m_categories, cat)
	{
		foreach (cat.directories, dir)
		{
			STATUS(GETSTR(IDS_SYNCINGDATABASE) + _T("... (") + 
					NumberToString((nDir++ * 100) / nTotalDirs) + _T("%)"));

			LOG(_T("Examining directory/list '") + dir.strPath + _T("'.\n"));

			// Check if it belongs to this computer

			if (!dir.strComputerName.IsEmpty() && 
					_tcsicmp(dir.strComputerName, GetComputerName()) != 0)
				continue;

			// Check if it's a directory or a list

			strPath = CorrectPath(dir.strPath);

			if (DirectoryExists(strPath))
			{
				// Remove movies that do no longer exist, are a directory when these shouldn't be 
				// indexed or are files that don't have the correct extension

				foreach (dir.movies, mov, i)
				{
					strPath = CorrectPath(dir.strPath + _T("\\") + mov.strFileName);

					bFileExists = FileExists(strPath);
					bDirExists = DirectoryExists(strPath);

					if ((!bFileExists && !bDirExists) || (bDirExists && !bIndexDirs) ||
							(bFileExists && strIndexExtensions.FindNoCase(_T("|") + 
							GetFileExt(mov.strFileName) + _T("|")) == -1))
						{dir.movies.RemoveAt(i); i--; nRemoved++;}
				}

				// Add new movies that do comply with the rules

				strPath = CorrectPath(dir.strPath);
				RObArray<FILEINFO> fileInfos = EnumFiles(strPath, _T("*"), bIndexDirs);

				foreach (fileInfos, fi)
				{
					if (!fi.bDirectory && strIndexExtensions.FindNoCase(_T("|") + 
							GetFileExt(fi.strName) + _T("|")) == -1)
						continue;

					pMov = NULL;
					foreach (dir.movies, mov)
						if (_tcsicmp(mov.strFileName, fi.strName) == 0)
							pMov = &mov;

					if (!pMov)
					{
						pMov = dir.movies.AddNew();
						ClearMovie(pMov);
						pMov->pDirectory = &dir;
						++nAdded;
					}

					pMov->strFileName = fi.strName;
					pMov->fileSize = fi.size;
					pMov->fileTime = fi.lastWriteTime;
				}
			}
			else if (FileExists(strPath)) // it's a movie list
			{
				RString str;
				if (!FileToString(strPath, str))
					{LOG(_T("Failed to read list '") + dir.strPath + _T("'.\n")); 
							ASSERT(false); continue;}
				FixLineEnds(str);

				RArray<const TCHAR*> names = SplitString(str, _T("\r\n"), true);

				// Remove movies that are no longer in list

				foreach (dir.movies, mov, i)
				{
					bool bRemove = true;
					foreach (names, name)
						if (_tcsicmp(mov.strFileName, name) == 0)
							{bRemove = false; break;}
					if (bRemove)
						{dir.movies.RemoveAt(i); i--; nRemoved++;}
				}

				// Add new movies
						
				foreach (names, name)
				{
					pMov = NULL;
					foreach (dir.movies, mov)
						if (_tcsicmp(name, mov.strFileName) == 0)
							{pMov = &mov; break;}

					if (!pMov)
					{
						pMov = dir.movies.AddNew();
						ClearMovie(pMov);
						pMov->pDirectory = &dir;
						++nAdded;
					}

					pMov->strFileName = name;
					pMov->fileSize = 0;
					pMov->fileTime = 0;
				}
			}
			else
				LOG(_T("Directory/list does not exist, failed to synchronize.\n"));

			// Update the list after processing a directory

			//Filter();

			//MSG msg;
			//while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			//{
			//	TranslateMessage(&msg);
			//	DispatchMessage(&msg);
			//}
		}
	}

	// Synchronizing successful, finalize

	LOG(_T("Done, ") + NumberToString(nAdded) + _T(" movies were added and ") + 
			NumberToString(nRemoved) + _T(" removed.\n"));
	STATUS(GETSTR(IDS_READY));

	Filter();
	Update();
}

void CDatabase::Update()
{
	LOG(_T("\nCollecting and updating movie information...\n"));
	STATUS(GETSTR(IDS_UPDATINGDATABASE) + _T("... (0%)"));
	
	CancelUpdate();

	m_updateMovies.SetSize(0);
	foreach (m_categories, cat)
		foreach (cat.directories, dir)
			foreach (dir.movies, mov)
				if (!mov.bUpdated)
					m_updateMovies.Add(&mov);

	m_nTotalUpdates = m_updateMovies;
	m_nUpdatesFromWeb = 0;

	if (m_nTotalUpdates == 0)
	{
		LOG(_T("Done, 0 updated required.\n"));
		STATUS(GETSTR(IDS_READY));
		return;
	}
	
	for (INT_PTR i = 0; i < min(m_updateMovies, DB_THREAD_COUNT); ++i)
	{
		UPDATETHREADDATA threadData;
		threadData.hDatabaseWnd = m_hWnd;
		UINT idThread;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, &threadData, 0, &idThread);
		ASSERT(hThread);
		VERIFY(WaitForSingleObject(threadData.eReady, DB_THREAD_WAIT) == WAIT_OBJECT_0);
		m_updateThreads.Add(hThread);
		m_updateThreadIDs.Add(idThread);
	}
}

void CDatabase::Update(DBMOVIE *pMov)
{
	m_updateMovies.Add(pMov);

	if (m_updateThreads == 0)
	{
		UPDATETHREADDATA threadData;
		threadData.hDatabaseWnd = m_hWnd;
		UINT idThread;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, &threadData, 0, &idThread);
		ASSERT(hThread);
		VERIFY(WaitForSingleObject(threadData.eReady, DB_THREAD_WAIT) == WAIT_OBJECT_0);
		m_updateThreads.Add(hThread);
		m_updateThreadIDs.Add(idThread);
	}
}

void CDatabase::CancelUpdate()
{
	if (m_updateThreads == 0)
		return;

	foreach (m_updateThreadIDs, idThread)
		PostThreadMessage(idThread, WM_QUIT);
	m_updateThreadIDs.SetSize(0);
	
	foreach (m_updateThreads, hThread)
		VERIFY(WaitForSingleObject(hThread, DB_THREAD_WAIT) == WAIT_OBJECT_0);
	m_updateThreads.SetSize(0);

	m_updateMovies.SetSize(0);

	LOG(_T("Canceled updating process.\n"));
	STATUS(GETSTR(IDS_READY));
}

LRESULT CDatabase::WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case DBM_GETMOVIEUPDATE:
		{
			if (!wParam || !lParam)
				ASSERTRETURN(FALSE);
			if (!m_updateMovies)
				return FALSE;
			DBMOVIE *pMov = (DBMOVIE*)wParam;
			*pMov = *m_updateMovies[0];
			DBMOVIE **ppOrigMov = (DBMOVIE**)lParam;
			*ppOrigMov = m_updateMovies[0];
			m_updateMovies.RemoveAt(0);
			return TRUE;
		}
		case DBM_SETMOVIEUPDATE:
		{
			if (!wParam || !lParam)
				ASSERTRETURN(FALSE);
			DBMOVIE *pMov = (DBMOVIE*)wParam;
			DBMOVIE *pOrigMov = (DBMOVIE*)lParam;
			*pOrigMov = *pMov;
			if (m_updateMovies)
				STATUS(GETSTR(IDS_UPDATINGDATABASE) + _T("... (") + 
						NumberToString(((m_nTotalUpdates - m_updateMovies) * 100) / 
						m_nTotalUpdates) + _T("%)"));
			Filter();
			return TRUE;
		}
		case DBM_UPDATETHREADEND:
		{
			if (!wParam)
				ASSERTRETURN(FALSE);
			foreach (m_updateThreadIDs, threadID, i)
			{
				if (threadID != (DWORD)wParam)
					continue;
				VERIFY(WaitForSingleObject(m_updateThreads[i], DB_THREAD_WAIT) == WAIT_OBJECT_0);
				m_updateThreadIDs.RemoveAt(i);
				m_updateThreads.RemoveAt(i);
				break;
			}
			m_nUpdatesFromWeb += lParam;
			if (m_updateThreads == 0 && m_updateMovies == 0)
			{
				LOG(_T("Done, performed ") + NumberToString(m_nUpdatesFromWeb) + 
						_T(" updates from the web.\n"));
				STATUS(GETSTR(IDS_READY));
				m_nTotalUpdates = 0;
				m_nUpdatesFromWeb = 0;
			}
			return TRUE;
		}
	}
	return RWindow::WndProc(Msg, wParam, lParam);
}

void CDatabase::OnPrefChanged()
{
	m_bShowSeenMovies = GETPREFBOOL(_T("ShowSeenMovies"));
	m_bShowHiddenMovies = GETPREFBOOL(_T("ShowHiddenMovies"));
	m_bSearchStoryline = GETPREFBOOL(_T("Search"), _T("Storyline"));
	m_sortBy = GETPREFINT(_T("SortBy")) + ID_SORT_TITLEASC;
	// don't call Filter here, that should be done from CReBar (possible text in search field)
}

void CDatabase::FilterByKeywords(RObArray<RString> keywords)
{
	m_filterKeywords = keywords;
	Filter();
}

void CDatabase::FilterByGenres(RObArray<RString> genres)
{
	m_filterGenres = genres;
	Filter();
}

void CDatabase::FilterByCategories(RArray<INT_PTR> categories)
{
	m_filterCategories = categories;
	Filter();
}

void CDatabase::Filter()
{
	m_movies.SetSize(0);

	RArray<DBMOVIE*> movies2;	// to be filled with less significant results
	RArray<DBMOVIE*> *pAddTo;
	RString str1, str2;
	bool bInserted;

	foreach (m_categories, cat, i)
	{
		if (m_filterCategories && m_filterCategories.IndexOf(i) == -1)
			continue;

		foreach (cat.directories, dir)
		{
			foreach (dir.movies, mov)
			{
				if (mov.bSeen && !m_bShowSeenMovies)
					continue;

				if (mov.bHide && !m_bShowHiddenMovies)
					continue;

				if (m_filterKeywords)
				{
					// Find significant match (file name, title and year)

					pAddTo = NULL;

					foreach (m_filterKeywords, strKeyword)
					{
						if (mov.strFileName.FindNoCase(strKeyword) != -1 ||
								mov.strTitle.FindNoCase(strKeyword) != -1 ||
								mov.strYear.FindNoCase(strKeyword) != -1)
							{pAddTo = &m_movies; break;}
					}

					// Find less significant match (the rest)

					if (!pAddTo)
					{
						foreach (m_filterKeywords, strKeyword)
						{
							if (mov.strGenres.FindNoCase(strKeyword) != -1 ||
									mov.strCountries.FindNoCase(strKeyword) != -1 ||
									mov.strDirectors.FindNoCase(strKeyword) != -1 ||
									mov.strWriters.FindNoCase(strKeyword) != -1 ||
									mov.strStars.FindNoCase(strKeyword) != -1 ||
									(m_bSearchStoryline && mov.strStoryline.FindNoCase(strKeyword) != -1))
								{pAddTo = &movies2; break;}
						}
					}
				}
				else
					pAddTo = &m_movies;

				// Sorted add/insert of movie to the array pointed to by pAddTo

				if (pAddTo)
				{
					switch (m_sortBy)
					{
					case ID_SORT_TITLEASC:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
							str2.MakeLower();

							if (_tcsicmp(str1, str2) < 0)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					case ID_SORT_TITLEDES:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
							str2.MakeLower();

							if (_tcsicmp(str1, str2) > 0)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					case ID_SORT_YEARASC:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							if (mov.nYear == pMov->nYear)
							{
								str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
								str2.MakeLower();

								if (_tcsicmp(str1, str2) < 0)
									{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
							}
							else if (mov.nYear < pMov->nYear)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					case ID_SORT_YEARDES:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							if (mov.nYear == pMov->nYear)
							{
								str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
								str2.MakeLower();

								if (_tcsicmp(str1, str2) < 0)
									{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
							}
							else if (mov.nYear > pMov->nYear)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					case ID_SORT_RATINGASC:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							if (mov.fRating == pMov->fRating)
							{
								str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
								str2.MakeLower();

								if (_tcsicmp(str1, str2) < 0)
									{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
							}
							else if (mov.fRating < pMov->fRating)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					case ID_SORT_RATINGDES:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							if (mov.fRating == pMov->fRating)
							{
								str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
								str2.MakeLower();

								if (_tcsicmp(str1, str2) < 0)
									{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
							}
							else if (mov.fRating > pMov->fRating)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					case ID_SORT_FILETIMEASC:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							if (mov.fileTime == pMov->fileTime)
							{
								str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
								str2.MakeLower();

								if (_tcsicmp(str1, str2) < 0)
									{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
							}
							else if (mov.fileTime > pMov->fileTime)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					case ID_SORT_FILETIMEDES:
						str1 = (mov.strTitle.IsEmpty() ? mov.strFileName : mov.strTitle);
						str1.MakeLower();

						bInserted = false;

						foreach (*pAddTo, pMov, i)
						{
							if (mov.fileTime == pMov->fileTime)
							{
								str2 = (pMov->strTitle.IsEmpty() ? pMov->strFileName : pMov->strTitle);
								str2.MakeLower();

								if (_tcsicmp(str1, str2) < 0)
									{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
							}
							else if (mov.fileTime < pMov->fileTime)
								{pAddTo->InsertAt(i, &mov); bInserted = true; break;}
						}

						if (!bInserted)
							pAddTo->Add(&mov);

						break;

					default:
						ASSERT(false);
					}
				}
			}
		}
	}

	m_movies.Add((const RArray<DBMOVIE*>&)movies2);

	MSG msg;
	if (!PeekMessage(&msg, GetMainWnd(), WM_DBUPDATED, WM_DBUPDATED, PM_NOREMOVE))
		PostMessage(GetMainWnd(), WM_DBUPDATED);
}
