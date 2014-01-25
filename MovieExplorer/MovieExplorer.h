#pragma once

#define LOG_MESSAGE		1
#define LOG_WARNING		2
#define LOG_ERROR		3

#define LOG(x, ...)		Log((RString)x, __VA_ARGS__)
#define STATUS			Status

#include "Database.h"

RPreferencesMgr2* GetPrefMgr();
RLanguageMgr* GetLangMgr();
RThemeMgr2* GetThemeMgr();
HWND GetMainWnd();
CDatabase* GetDB();
void Log(const TCHAR *lpszText, UINT_PTR type = LOG_MESSAGE);
void Status(const TCHAR *lpszText);

#include "resource.h"
#include "strings.h"
#include "messages.h"
