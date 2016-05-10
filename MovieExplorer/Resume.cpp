#include "stdafx.h"
#include "MovieExplorer.h"
#include "Resume.h"
#include <Windows.h>
#include <lmcons.h>


struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};


Resume::Resume()
{
	size = 0;
	info = { sizeof(info) };

}


bool Resume::ReadVlcResumeFile()
{

	//get username
	TCHAR username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName(username, &username_len);

	RString strTemp;
	RString strVlcFile;

	//build path to vlc info file
	RString strFilePath = _T("C:\\Users\\");
	strFilePath += username;
	strFilePath += _T("\\AppData\\Roaming\\vlc\\vlc-qt-interface.ini");

	//read VLC resume file into str
	if (!FileToString(strFilePath, strVlcFile))
		return false;

	if (GetFirstMatch(strVlcFile, _T("list=([^$]*?$)"), &strTemp))
	{
		strTemp.Replace(_T(" "), _T(""));
		RArray<const TCHAR*> moviesTemp = SplitString(strTemp, _T(","), true);
		size = moviesTemp.GetSize();
		if (size > MAX_SIZE) size = MAX_SIZE;

		for(int i=0; i<size; i++)
		{
			//remove % codes and turn + to space
			RString strTempMovie = URLDecode(moviesTemp[i]);

			//remove file:/// or file://
			if (strTempMovie.Left(8) == _T("file:///"))
				strTempMovie = strTempMovie.Right(strTempMovie.GetLength() - 8);
			else
				strTempMovie = strTempMovie.Right(strTempMovie.GetLength() - 5);

			strTempMovie.Replace(_T("/"), _T("\\\\"));
			movies[i] = strTempMovie;
		}
	}

	if (GetFirstMatch(strVlcFile, _T("times=([^$]*?$)"), &strTemp))
	{
		strTemp.Replace(_T(" "), _T(""));
		RArray<const TCHAR*> strTimes = SplitString(strTemp, _T(","), true);
		for(int i=0; i<strTimes.GetSize(); i++)
			times[i] = StringToNumber(strTimes[i])/1000;  //milliseconds to seconds
	}
}

int Resume::GetTime(RString str)
{
	//If movie path str is in the list return its time.
	//Otherwise return 0

	str.Replace(_T("\\"), _T("\\\\"));

	for(int i=0; i<size; i++)
	{
		if (str == movies[i])
			return(times[i]);
	}
	
	return 0; 
}

void Resume::CloseVlc()
{
	HWND h = find_main_window(processInfo.dwProcessId);
	SendMessage(h, WM_CLOSE, NULL, NULL);
//	CloseHandle(processInfo.hThread);
//	CloseHandle(processInfo.hProcess);
}


void Resume::LaunchVlc(RString strFilePath)
{
	RString path = _T("C:\\Program Files (x86)\\VideoLAN\\VLC\\vlc.exe ");
	RString cmd = path + strFilePath + _T(" --play-and-exit --start-time=") + NumberToString(GetTime(strFilePath));

	TCHAR* param = new TCHAR[cmd.GetLength() + 1];
	_tcscpy(param, cmd);

	CreateProcess(NULL, param, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo);
	delete(param);
}

//Private


BOOL Resume::is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK Resume::enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !is_main_window(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
}

HWND Resume::find_main_window(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.best_handle;
}
