#include "stdafx.h"
#include "MovieExplorer.h"
#include "GeneralPage.h"

CGeneralPage::CGeneralPage()
{
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::ApplyChanges()
{
	// Change language

	RString strFilePath = m_langFiles[m_cbLanguage.GetSel()].strFilePath;
	RString strRelFilePath = RelativePath(strFilePath);
	if (!strRelFilePath.EqualsNoCase(GETPREFSTR(_T("LanguageFile"))))
	{
		RString strEnglishName = m_langFiles[m_cbLanguage.GetSel()].strName;
		RRegEx re(_T(".+ \\(([^)]+)\\)"));
		if (re.Search(strEnglishName))
			strEnglishName = re.GetMatch(1);
		if (GetLangMgr()->SelectLanguage(strEnglishName))
			SETPREFSTR(_T("LanguageFile"), strRelFilePath);
		else
			ASSERT(false);
	}

	// Change theme

	strFilePath = m_themeFiles[m_cbTheme.GetSel()].strFilePath;
	strRelFilePath = RelativePath(strFilePath);
	if (!strRelFilePath.EqualsNoCase(GETPREFSTR(_T("ThemeFile"))))
	{
		if (GetThemeMgr()->Open(strFilePath))
			SETPREFSTR(_T("ThemeFile"), RelativePath(strFilePath));
		else
			ASSERT(false);
	}

	// The rest

	SETPREFBOOL(_T("NormalizeRatings"), (bool)m_chkNormalizeRatings.GetCheck());
	SETPREFBOOL(_T("Search"), _T("Instantly"), (bool)m_chkSearchInstantly.GetCheck());
	SETPREFBOOL(_T("Search"), _T("Literally"), (bool)m_chkSearchLiterally.GetCheck());
	SETPREFBOOL(_T("Search"), _T("Storyline"), (bool)m_chkSearchStoryline.GetCheck());

	RPropertyPage::ApplyChanges();
}

bool CGeneralPage::OnCreate(CREATESTRUCT *pCS)
{
	if (!RPropertyPage::OnCreate(pCS))
		ASSERTRETURN(false);

	// Create controls

	DWORD cbStyle = CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|CBS_HASSTRINGS|WS_VSCROLL|WS_TABSTOP;

	if (
			!m_cbLanguage.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_stcLanguage.Create<RStatic>(m_hWnd) ||
			!m_cbTheme.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_stcTheme.Create<RStatic>(m_hWnd) ||
			!m_chkNormalizeRatings.Create<RButton>(m_hWnd, BS_AUTOCHECKBOX|WS_TABSTOP) ||
			!m_grpInterface.Create<RButton>(m_hWnd, BS_GROUPBOX) ||

			!m_chkSearchInstantly.Create<RButton>(m_hWnd, BS_AUTOCHECKBOX|WS_TABSTOP) ||
			!m_chkSearchLiterally.Create<RButton>(m_hWnd, BS_AUTOCHECKBOX|WS_TABSTOP) ||
			!m_chkSearchStoryline.Create<RButton>(m_hWnd, BS_AUTOCHECKBOX|WS_TABSTOP) ||
			!m_grpSearch.Create<RButton>(m_hWnd, BS_GROUPBOX)
		)
		ASSERTRETURN(false);

	// Populate language combo box

	m_langFiles.SetSize(0);

	RString strDir = GetDir(CorrectPath(GETPREFSTR(_T("LanguageFile")), true));
	RObArray<RString> fileNames = EnumFileNames(strDir, _T("*.xml"));
	RString strFilePath;
	RXMLFile xmlFile;
	RXMLTag *pMainTag;
	CONFIGFILE *pCF;
	INT_PTR nSelected;

	foreach (fileNames, strFileName)
	{
		strFilePath = strDir + _T("\\") + strFileName;

		if (!xmlFile.Read(strFilePath) || 
				!(pMainTag = xmlFile.GetRootTag()->GetChild(_T("LanguageFile"))) ||
				(RString)pMainTag->GetProperty(_T("applicationID")) != APPLICATION_ID ||
				!pMainTag->GetChild(_T("Name")) ||
				!pMainTag->GetChild(_T("EnglishName")))
			continue;

		pCF = m_langFiles.AddNew();
		pCF->strFilePath = strFilePath;
		pCF->strName = pMainTag->GetChildContent(_T("Name"));

		if (_tcsicmp(pMainTag->GetChildContent(_T("EnglishName")), _T("English")) != 0)
			pCF->strName += (RString)_T(" (") + pMainTag->GetChildContent(_T("EnglishName")) + _T(")");

		if (_tcsicmp(strFilePath, CorrectPath(GETPREFSTR(_T("LanguageFile")), true)) == 0)
			nSelected = m_langFiles-1;
	}

	foreach (m_langFiles, cf)
		m_cbLanguage.Add(cf.strName);

	m_cbLanguage.SetSel(nSelected);

	// Populate theme combo box

	m_themeFiles.SetSize(0);

	strDir = GetDir(CorrectPath(GETPREFSTR(_T("ThemeFile")), true));
	fileNames = EnumFileNames(strDir, _T("*.xml"));
	nSelected = -1;

	foreach (fileNames, strFileName)
	{
		strFilePath = strDir + _T("\\") + strFileName;

		if (!xmlFile.Read(strFilePath) || 
				!(pMainTag = xmlFile.GetRootTag()->GetChild(_T("ThemeFile"))) ||
				/*(RString)pMainTag->GetProperty(_T("applicationID")) != APPLICATION_ID ||*/
				!pMainTag->GetChild(_T("Name")))
			continue;

		pCF = m_themeFiles.AddNew();
		pCF->strFilePath = strFilePath;
		pCF->strName = pMainTag->GetChildContent(_T("Name"));

		if (_tcsicmp(strFilePath, CorrectPath(GETPREFSTR(_T("ThemeFile")), true)) == 0)
			nSelected = m_themeFiles-1;
	}

	foreach (m_themeFiles, cf)
		m_cbTheme.Add(cf.strName);

	m_cbTheme.SetSel(nSelected);

	// Load settings

	m_chkNormalizeRatings.SetCheck(GETPREFBOOL(_T("NormalizeRatings")));

	m_chkSearchInstantly.SetCheck(GETPREFBOOL(_T("Search"), _T("Instantly")));
	m_chkSearchLiterally.SetCheck(GETPREFBOOL(_T("Search"), _T("Literally")));
	m_chkSearchStoryline.SetCheck(GETPREFBOOL(_T("Search"), _T("Storyline")));

	// Load control text

	OnPrefChanged();

	return true;
}

void CGeneralPage::OnSize(DWORD type, WORD cx, WORD cy)
{
	RPropertyPage::OnSize(type, cx, cy);

	int y = DUY(4);
	MoveWindow(m_grpInterface, DUX(4), y, cx - DUX(8), DUY(56));
	y += DUY(12);
	MoveStatic(m_stcLanguage, DUX(14), y+DUY(2));
	MoveWindow(m_cbLanguage, DUX(54), y, DUX(90), DUY(12));
	y += DUY(14);
	MoveStatic(m_stcTheme, DUX(14), y+DUY(2));
	MoveWindow(m_cbTheme, DUX(54), y, DUX(90), DUY(12));
	y += DUY(16);
	MoveCheckBox(m_chkNormalizeRatings, DUX(14), y);

	y = DUY(64);
	MoveWindow(m_grpSearch, DUX(4), y, cx - DUX(8), DUY(54));
	y += DUY(12);
	MoveCheckBox(m_chkSearchInstantly, DUX(14), y);
	y += DUY(14);
	MoveCheckBox(m_chkSearchLiterally, DUX(14), y);
	y += DUY(14);
	MoveCheckBox(m_chkSearchStoryline, DUX(14), y);

	PostMessage(m_hWnd, WM_PAINT);
	PostChildren(m_hWnd, WM_PAINT);
}

void CGeneralPage::OnPrefChanged()
{
	RPropertyPage::OnPrefChanged();

	m_stcLanguage.SetText(GETSTR(IDS_LANGUAGE) + _T(":"));
	m_stcTheme.SetText(GETSTR(IDS_THEME) + _T(":"));
	m_chkNormalizeRatings.SetText(_T(" ") + GETSTR(IDS_NORMALIZERATINGS));
	m_grpInterface.SetText(GETSTR(IDS_INTERFACE));

	m_chkSearchInstantly.SetText(_T(" ") + GETSTR(IDS_SEARCHINSTANTLY));
	m_chkSearchLiterally.SetText(_T(" ") + GETSTR(IDS_SEARCHLITERALLY));
	m_chkSearchStoryline.SetText(_T(" ") + GETSTR(IDS_SEARCHSTORYLINE));
	m_grpSearch.SetText(GETSTR(IDS_SEARCH));

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
}

void CGeneralPage::OnDrawItem(UINT_PTR id, DRAWITEMSTRUCT *pDIS)
{
	RPropertyPage::OnDrawItem(id, pDIS);

	RString strText;
	INT_PTR nLen = SendMessage(pDIS->hwndItem, CB_GETLBTEXTLEN, (WPARAM)pDIS->itemID);
	if (nLen == CB_ERR)
		return;
	SendMessage(pDIS->hwndItem, CB_GETLBTEXT, (WPARAM)pDIS->itemID, (LPARAM)strText.GetBuffer(nLen));
	strText.ReleaseBuffer(nLen);

	SetBkColor(pDIS->hDC, (pDIS->itemState & ODS_SELECTED ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_WINDOW)));
	ExtTextOut(pDIS->hDC, 0, 0, ETO_OPAQUE, &pDIS->rcItem, NULL, 0, NULL);
	SetTextColor(pDIS->hDC, (pDIS->itemState & ODS_SELECTED ? GetSysColor(COLOR_HIGHLIGHTTEXT) : 
			(pDIS->itemState & ODS_DISABLED ? GetSysColor(COLOR_GRAYTEXT) : GetSysColor(COLOR_WINDOWTEXT))));
	TextOut(pDIS->hDC, DUX(1) + pDIS->rcItem.left, pDIS->rcItem.top, strText);
}

void CGeneralPage::OnMeasureItem(UINT_PTR id, MEASUREITEMSTRUCT *pMIS)
{
	RPropertyPage::OnMeasureItem(id, pMIS);
	pMIS->itemHeight = DUY(8);
}

void CGeneralPage::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	RPropertyPage::OnCommand(notifyCode, id, hWndControl);

	if (notifyCode == CBN_SELENDOK || notifyCode == BN_CLICKED)
		SetChanged();
}
