#include "stdafx.h"
#include "MovieExplorer.h"

bool CorrectPreferences()
{
	RPreferencesMgr2 prefMgr;
	prefMgr.Open(CorrectPath(_T("Preferences.xml")));

	prefMgr.SetStr(_T("LanguageFile"), _T("Languages\\English.xml"), false);

	if (IsWin8())
		prefMgr.SetStr(_T("ThemeFile"), _T("Themes\\Win8.xml"), false);
	else
		prefMgr.SetStr(_T("ThemeFile"), _T("Themes\\Light.xml"), false);

	prefMgr.SetStr(_T("DatabaseFile"), _T("Database.xml"), false);
	
	// TODO: determine max window dimension using GetSystemMetrics or something else

	prefMgr.SetInt(_T("MainWnd"), _T("x"), 150, false);
	prefMgr.SetInt(_T("MainWnd"), _T("y"), 30, false);
	prefMgr.SetInt(_T("MainWnd"), _T("cx"), 950, false);
	prefMgr.SetInt(_T("MainWnd"), _T("cy"), 750, false);
	prefMgr.SetBool(_T("MainWnd"), _T("Maximized"), false, false);

	if (prefMgr.GetInt(_T("MainWnd"), _T("x")) + prefMgr.GetInt(_T("MainWnd"), _T("cx")) < 10)
	{
		prefMgr.SetInt(_T("MainWnd"), _T("x"), 0);
		prefMgr.SetInt(_T("MainWnd"), _T("cx"), 900);
	}
	if (prefMgr.GetInt(_T("MainWnd"), _T("y")) + prefMgr.GetInt(_T("MainWnd"), _T("cy")) < 10)
	{
		prefMgr.SetInt(_T("MainWnd"), _T("y"), 0);
		prefMgr.SetInt(_T("MainWnd"), _T("cy"), 600);
	}

	prefMgr.SetFloat(_T("MainWnd"), _T("Zoom"), GetScale(), false);
	SetScale(prefMgr.GetFloat(_T("MainWnd"), _T("Zoom")));

	prefMgr.SetBool(_T("MainWnd"), _T("ShowStatusBar"), true, false);
	prefMgr.SetBool(_T("MainWnd"), _T("ShowLog"), true, false);
	prefMgr.SetInt(_T("MainWnd"), _T("LogHeight"), 108, false);

	prefMgr.SetStr(_T("Database"), _T("IndexExtensions"), _T("asf|avi|mkv|mp4|mpeg|mpg|wmv"), false);
	prefMgr.SetBool(_T("Database"), _T("IndexDirectories"), true, false);
	prefMgr.SetInt(_T("Database"), _T("MaxInfoAge"), 2);
	prefMgr.SetStr(_T("Database"), _T("CacheDirectory"), _T("Cache"), false);

	prefMgr.SetStr(_T("InfoService"), _T("OnlyUse"), _T("imdb.com"), false);
	//prefMgr.SetStr(_T("InfoService"), _T("Backup"), _T(""), false);
	prefMgr.SetStr(_T("InfoService"), _T("Title"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Year"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Genres"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Countries"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Runtime"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Storyline"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Directors"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Writers"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Stars"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Poster"), _T("imdb.com"), false);
	prefMgr.SetStr(_T("InfoService"), _T("Rating"), _T("imdb.com"), false);

	prefMgr.SetBool(_T("Search"), _T("Instantly"), true, false);
	prefMgr.SetBool(_T("Search"), _T("Literally"), false, false);
	prefMgr.SetBool(_T("Search"), _T("Storyline"), false, false);

	prefMgr.SetBool(_T("NormalizeRatings"), false, false);
	prefMgr.SetBool(_T("ShowSeenMovies"), false, false);
	prefMgr.SetBool(_T("ShowHiddenMovies"), false, false);
	prefMgr.SetInt(_T("SortBy"), 0, false);
	prefMgr.SetBool(_T("IMDbUseBingSeach"), true, false);
	prefMgr.SetBool(_T("IMDbUseOriginalTitle"), false, false);
	
	if (!prefMgr.SaveAs(CorrectPath(_T("Preferences.xml"))))
		ASSERTRETURN(false);

	return true;
}