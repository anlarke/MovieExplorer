#pragma once

#define APPLICATION_ID _T("MOVIEEXPLORER083")

#define _WIN32_WINNT			0x501	// target WinXP and higher
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN				// exclude rarely used stuff from the Windows headers

#define _CRT_SECURE_NO_WARNINGS			// not using the safe string functions
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "..\RClasses\debug.h"//#include <stdlib.h>
#include <commctrl.h>
#include <objbase.h>
#include <locale.h>
#include <new>
#include <wincodec.h>
#include <cmath>
#include <uxtheme.h>
#include <vsstyle.h>
#include <olectl.h>
#include <process.h>
#include <shlobj.h>
#include <shellapi.h>
#include <winhttp.h>

// RClasses

#include "..\RClasses\scale.h"			// basic functions that do not require RClasses
#include "..\RClasses\messages.h"
#include "..\RClasses\wcsistr.h"
#include "..\RClasses\time.h"
#include "..\RClasses\move.h"
#include "..\RClasses\foreach.h"

#include "..\RClasses\RArray.h"			// basic classes that do not require other RClasses
#include "..\RClasses\RString.h"
#include "..\RClasses\RMemoryDC.h"
#include "..\RClasses\RSaveDC.h"
#include "..\RClasses\RSprite.h"
#include "..\RClasses\RFont.h"
#include "..\RClasses\RCriticalSection.h"
#include "..\RClasses\RComPtr.h"
#include "..\RClasses\RRect.h"
#include "..\RClasses\RSocket.h"

#include "..\RClasses\general.h"		// functions that require RClasses
#include "..\RClasses\convert.h"
#include "..\RClasses\httphelp.h"
#include "..\RClasses\draw.h"
#include "..\RClasses\mltextout.h"
#include "..\RClasses\capturem.h"

#include "..\RClasses\RWindow.h"		// classes that require other RClasses
#include "..\RClasses\RControl.h"
#include "..\RClasses\RComboBox.h"
#include "..\RClasses\RDialog.h"
#include "..\RClasses\RStatic.h"
#include "..\RClasses\RButton.h"
#include "..\RClasses\RTabControl.h"
#include "..\RClasses\REdit.h"
#include "..\RClasses\RHintEdit.h"
#include "..\RClasses\RScrollBar.h"
#include "..\RClasses\RCustomToolBarButton.h"
#include "..\RClasses\RPropertySheet.h"
#include "..\RClasses\RXML.h"
#include "..\RClasses\RXML2.h"
#include "..\RClasses\RPreferencesMgr2.h"
#include "..\RClasses\RLanguageMgr.h"
#include "..\RClasses\RThemeMgr2.h"
#include "..\RClasses\RRegEx_boost.h"
#include "..\RClasses\RTreeView.h"
#include "..\RClasses\RCustomToolBar.h"
