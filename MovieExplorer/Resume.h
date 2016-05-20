#pragma once

#define MAX_SIZE 32

class Resume
{
	public:

		PROCESS_INFORMATION processInfo;
		STARTUPINFO info;

		RString movies[MAX_SIZE];
		int times[MAX_SIZE];
		int size;

		Resume();
		void ReadThread();
		void ReadVlcResumeFile();
		int GetTime(RString str);
		void UpdateResumeTimes();
		void CloseVlc();
		void LaunchVlc(RString strFileName, UINT64 resumeTime);

	private:
		static BOOL is_main_window(HWND handle);
		static BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam);
		static HWND find_main_window(unsigned long process_id);

};
