#define WINVER			0x0400
#define _WIN32_WINNT	0x0400
#define _WIN32_WINDOWS	0x0400

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "..\RClasses\debug.h"
#include <locale.h>
#include <new>

#include "..\RClasses\move.h"
#include "..\RClasses\RString.h"
#include "..\RClasses\RArray.h"
#include "..\RClasses\general.h"
#include "..\RClasses\convert.h"
#include "..\RClasses\RCSV.h"

#ifdef _DEBUG
#define PAUSERETURN(x) {system("pause"); return x;}
#else
#define PAUSERETURN(x) return x;
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	_tsetlocale(LC_ALL, _T("")); // fixes VS11DP quirk

	// open csv file

	RCSVFileRO file;

	if (argc == 2)
	{
		if (!file.Open(CorrectPath(argv[1])))
			{_putts(_T("Unable to open file: ") + CorrectPath(argv[1])); PAUSERETURN(-1);}
	}
	else if (argc == 1)
	{
		if (!file.Open(CorrectPath(_T("strings.csv"))))
			{_putts(_T("Please supply the name of CSV-file.")); PAUSERETURN(-2);}
	}
	else
		{_putts(_T("Invalid argument count.")); PAUSERETURN(-3);}

	if (file.GetLineCount() == 0)
		{_putts(_T("Empty file.")); PAUSERETURN(1);}

	if (file.GetLineCount() < 2)
		{_putts(_T("File must contain at least two lines.")); PAUSERETURN(-4);}

	if (file.GetFieldCount() < 2)
		{_putts(_T("File must contain at least two columns.")); PAUSERETURN(-5);}

	// generate header file contents

	RString str;
	str += _T("// strings.h : Defines the strings to be used throughout the application.\r\n");
	str += _T("//\r\n");
	str += _T("\r\n");

	int n = 1000;
	
	int i, j;
	for (i = 1; i < file.GetLineCount(); i++)
		if (_tcscmp(file.GetField(i, 0), _T("")) != 0)
			str += RString(_T("#define ")) + file.GetField(i, 0) + _T(" ") + NumberToString(n++) + _T("\r\n");

	str += _T("\r\n");
	str += _T("inline void LoadStrings()\r\n");
	str += _T("{\r\n");
	str += _T("\tINT_PTR nLanguage;\r\n");

	for (i = 1; i < file.GetFieldCount(); i++)
	{
		str += _T("\r\n");
		str += (RString)_T("\tnLanguage = GetLangMgr()->GetLanguage(_T(\"") + file.GetField(0, i) + _T("\"));\r\n");
		str += (RString)_T("\tif (nLanguage == -1)\r\n");
		str += (RString)_T("\t\tnLanguage = GetLangMgr()->AddLanguage(_T(\"") + file.GetField(0, i) + _T("\"), _T(\"") + file.GetField(1, i)
				+ _T("\"), _T(\"\"));\r\n\r\n");

		for (j = 1; j < file.GetLineCount(); j++)
		{
			if (_tcscmp(file.GetField(j, 0), _T("")) == 0)
				continue; // skip empty ids

			str += (RString)_T("\tGetLangMgr()->SetString(nLanguage, ") + NumberToString(j-1) + _T(", _T(\"") +
					file.GetField(j, i) + _T("\"), false);\r\n");
		}
	}

	str += _T("}\r\n");

	// write to file

	RString strFilePath = CorrectPath(argc == 2 ? argv[1] : _T("strings.csv"));
	strFilePath = strFilePath.Left(strFilePath.ReverseFind(_T('\\'))+1) + _T("strings.h");

	RArray<BYTE> data = StringToData(str, CHARSET_ANSI);
	if (!DataToFile(data, strFilePath))
		{_tprintf(_T("Failed to open %s for writing.\n"), (LPCTSTR)strFilePath); PAUSERETURN(-6);}

	_tprintf(_T("Created %s succesfully.\n"), (LPCTSTR)strFilePath);

	PAUSERETURN(0);
}
