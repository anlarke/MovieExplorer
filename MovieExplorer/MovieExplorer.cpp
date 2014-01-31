#include "stdafx.h"
#include "MovieExplorer.h"
#include "MainWnd.h"

#pragma comment(lib, "comctl32")
#pragma comment(lib, "msimg32")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winhttp")
#pragma comment(lib, "uxtheme")

bool CorrectPreferences();
bool CorrectThemes();

RPreferencesMgr2* g_pPrefMgr = NULL;
RPreferencesMgr2* GetPrefMgr() {ASSERTRETURN(g_pPrefMgr);}

RLanguageMgr* g_pLangMgr = NULL;
RLanguageMgr* GetLangMgr() {ASSERTRETURN(g_pLangMgr);}

RThemeMgr2* g_pThemeMgr = NULL;
RThemeMgr2* GetThemeMgr() {ASSERTRETURN(g_pThemeMgr);}

CDatabase* g_pDB = NULL;
CDatabase* GetDB() {ASSERTRETURN(g_pDB);}

HWND g_hMainWnd = NULL;
HWND GetMainWnd() {return g_hMainWnd;} // can be NULL

void Log(const TCHAR *lpszText, UINT_PTR type)
{
	static RCriticalSection sect;
	RLock lock(&sect);
	SendMessage(g_hMainWnd, WM_LOG_WRITE, (WPARAM)lpszText, (LPARAM)type);
}

void Status(const TCHAR *lpszText)
{
	static RCriticalSection sect;
	RLock lock(&sect);
	SendMessage(g_hMainWnd, WM_STATUS_WRITE, (WPARAM)lpszText);
}

void Run()
{
	// NOTE: Not using global objects, because otherwise their constructors are called before the 
	// required initialization of components takes place. Their lifetime has been limited to this
	// function so they are destructed before uninitialization of the components takes place.

	// Check for write access in application directory

	HANDLE hFile = CreateFile(CorrectPath(_T("test.txt")), GENERIC_READ|GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, _T("Failed to obtain write access to application directory."), 
			_T("Movie Explorer"), MB_ICONEXCLAMATION);
		return;
	}
	CloseHandle(hFile);
	DeleteFile(CorrectPath(_T("test.txt")));

	if (!CreateDirectory(CorrectPath(_T("test"))))
	{
		MessageBox(NULL, _T("Failed to obtain write access to application directory."), 
			_T("Movie Explorer"), MB_ICONEXCLAMATION);
		return;
	}
	RemoveDirectory(CorrectPath(_T("test")));

	// Load preferences

	RPreferencesMgr2 prefMgr;
	g_pPrefMgr = &prefMgr;
	if (!CorrectPreferences() || !prefMgr.Open(CorrectPath(_T("Preferences.xml"))))
	{
		MessageBox(NULL, _T("Failed to load preferences."), _T("Movie Explorer"), MB_ICONEXCLAMATION);
		return;
	}

	// Copy translations to disk

	if (!DirectoryExists(CorrectPath(_T("Languages"))))
		CreateDirectory(CorrectPath(_T("Languages")));

	HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDT_CROATIAN), _T("XMLFile"));
	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource(NULL, hResInfo);
		if (hResData)
		{
			DataToFile((const BYTE*)LockResource(hResData), 
					SizeofResource(GetModuleHandle(NULL), hResInfo), 
					CorrectPath(_T("Languages\\Croatian.xml")));
			UnlockResource(hResData);
		}
	}

	hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDT_ITALIAN), _T("XMLFile"));
	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource(NULL, hResInfo);
		if (hResData)
		{
			DataToFile((const BYTE*)LockResource(hResData), 
					SizeofResource(GetModuleHandle(NULL), hResInfo), 
					CorrectPath(_T("Languages\\Italian.xml")));
			UnlockResource(hResData);
		}
	}

	hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDT_GREEK), _T("XMLFile"));
	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource(NULL, hResInfo);
		if (hResData)
		{
			DataToFile((const BYTE*)LockResource(hResData), 
					SizeofResource(GetModuleHandle(NULL), hResInfo), 
					CorrectPath(_T("Languages\\Greek.xml")));
			UnlockResource(hResData);
		}
	}

	// Load language strings

	RLanguageMgr langMgr;
	g_pLangMgr = &langMgr;
	LoadStrings(); // TODO: Have a pre-build step that generates the XML-files from the CSV instead of this
	langMgr.Save(CorrectPath(_T("Languages")));
	langMgr.Load(CorrectPath(_T("Languages")));
	
	RString strLanguageEnglish = prefMgr.GetStr(_T("LanguageFile"));
	strLanguageEnglish.Replace(_T('/'), _T('\\'));
	INT_PTR m = strLanguageEnglish.ReverseFind(_T('\\')); // TODO: make it possible to load language files at different location
	INT_PTR n = strLanguageEnglish.ReverseFind(_T('.'));
	if (m == -1 || n == -1 || !langMgr.SelectLanguage(strLanguageEnglish.Mid(m+1, n-m-1)))
		VERIFY(langMgr.SelectLanguage(_T("English")));

	// Load theme

	RThemeMgr2 themeMgr;
	g_pThemeMgr = &themeMgr;
	if (!CorrectThemes() || !themeMgr.Open(CorrectPath(prefMgr.GetStr(_T("ThemeFile")))))
	{
		//prefMgr.SetString(_T("ThemeFile"), _T("Themes\\Light.xml"));
		//if (!themeMgr.Open(CorrectPath(_T("Themes\\Light.xml"))))
		//{
			MessageBox(NULL, _T("Failed to load theme."), _T("Movie Explorer"), MB_ICONEXCLAMATION);
			return;
		//}
	}

	// Create database object (required by main window)

	CDatabase db;
	g_pDB = &db;

	// Create and show main window

	CMainWnd mainWnd;
	if (!mainWnd.Create())
	{
		MessageBox(NULL, _T("Failed to create main window."), _T("Movie Explorer"), MB_ICONEXCLAMATION);
		return;
	}
	g_hMainWnd = mainWnd;

	// Update log and status

    LOG(_T("Movie Explorer - version 0.83f\n"));
    LOG(_T("built January 31, 2014\n"));
	STATUS(_T("Ready"));

	SendMessage(mainWnd, WM_PAINT);
	SendChildrenRec(mainWnd, WM_PAINT);

	// Load database

	RString strDatabaseFile = CorrectPath(prefMgr.GetStr(_T("DatabaseFile")));
	if (!db.Load(strDatabaseFile))
	{
		LOG(_T("Created default database file.\n"));

		RXMLFile xmlFile;
		RXMLTag *pCategoriesTag, *pCategoryTag, *pDirectoryTag;

		pCategoriesTag = xmlFile.GetRootTag()->AddChild(_T("DatabaseFile"));
		pCategoriesTag->SetProperty(_T("applicationID"), APPLICATION_ID);

		RString strMyVideosPath;
		SHGetFolderPath(NULL, CSIDL_MYVIDEO, NULL, SHGFP_TYPE_CURRENT, strMyVideosPath.GetBuffer(MAX_PATH));
		strMyVideosPath.ReleaseBuffer();
		if (strMyVideosPath.IsEmpty())
			strMyVideosPath = _T("C:\\My Videos");
		RString strMyVideos;
		INT_PTR n = strMyVideosPath.ReverseFind(_T('\\'));
		if (n != -1)
			strMyVideos = strMyVideosPath.Mid(n+1);
		else
			strMyVideos = _T("My Videos");

		pCategoryTag = pCategoriesTag->AddChild(_T("Category"));
		pCategoryTag->SetProperty(_T("name"), strMyVideos);
		pDirectoryTag = pCategoryTag->AddChild(_T("Directory"));
		pDirectoryTag->SetProperty(_T("path"), strMyVideosPath);
		pDirectoryTag->SetProperty(_T("computerName"), GetComputerName());

		xmlFile.Write(strDatabaseFile);
		VERIFY(db.Load(strDatabaseFile));
	}

	// Main message loop

	MSG msg;
	short nWheelZoom = 0;

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if (msg.message == WM_MOUSEWHEEL && GetKeyState(VK_CONTROL) < 0)
		{
			nWheelZoom += (short)HIWORD(msg.wParam);
			if (abs(nWheelZoom) >= WHEEL_DELTA)
			{
				if (nWheelZoom > 0)
				{
					msg.hwnd = mainWnd;
					msg.message = WM_COMMAND;
					msg.wParam = MAKEWPARAM(ID_TOOLS_ZOOMIN, 1);
					msg.lParam = 0;
				}
				else
				{
					msg.hwnd = mainWnd;
					msg.message = WM_COMMAND;
					msg.wParam = MAKEWPARAM(ID_TOOLS_ZOOMOUT, 1);
					msg.lParam = 0;
				}
				nWheelZoom = 0;
			}
		}
		else if (msg.message == WM_KEYDOWN)
		{
			if (GetKeyState(VK_CONTROL) < 0 && msg.wParam == '0')
			{
				msg.hwnd = mainWnd; 
				msg.message = WM_COMMAND; 
				msg.wParam = MAKEWPARAM(ID_TOOLS_ZOOMRESET, 1); 
				msg.lParam = 0;
			}
			else if (GetKeyState(VK_CONTROL) < 0 && msg.wParam == 'S')
			{
				msg.hwnd = mainWnd; 
				msg.message = WM_COMMAND; 
				msg.wParam = MAKEWPARAM(ID_TOOLS_STATUSBAR, 1); 
				msg.lParam = 0;
			}
			else if (GetKeyState(VK_CONTROL) < 0 && msg.wParam == 'L')
			{
				msg.hwnd = mainWnd; 
				msg.message = WM_COMMAND; 
				msg.wParam = MAKEWPARAM(ID_TOOLS_LOG, 1); 
				msg.lParam = 0;
			}
			else if (GetKeyState(VK_CONTROL) < 0 && msg.wParam == 'O')
			{
				msg.hwnd = mainWnd; 
				msg.message = WM_COMMAND; 
				msg.wParam = MAKEWPARAM(ID_TOOLS_OPTIONS, 1); 
				msg.lParam = 0;
			}
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Controlled destruction

	mainWnd.Destroy();
	g_hMainWnd = NULL; // otherwise the Log and Status functions will hang

	prefMgr.SetFloat(_T("MainWnd"), _T("Zoom"), GetScale());
	VERIFY(prefMgr.Save());

	db.Save();
	db.Unload(); // should unload while pref and lang mgr still available

	// Clear global pointers

	g_pPrefMgr = NULL;
	g_pLangMgr = NULL;
	g_pThemeMgr = NULL;
	g_pDB = NULL;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// Component initialization

	DEBUG_INIT();
	SCALE_INIT();
	SOCKET_INIT();
	CAPTUREM_INIT();
	_tsetlocale(LC_ALL, _T(""));
	srand((UINT)time(NULL));
	InitCommonControls();
	VERIFY(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE)));
	
	// Run the application

	Run();

	// Uninitialize components
	
	CoUninitialize();
	CAPTUREM_UNINIT();
	SOCKET_UNINIT();
	
	return 0;
}
