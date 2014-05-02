#include "stdafx.h"
#include "MovieExplorer.h"
#include "DatabasePage.h"

CDatabasePage::CDatabasePage()
{
}

CDatabasePage::~CDatabasePage()
{
}

void CDatabasePage::ApplyChanges()
{
	bool bRequireSync = false;
	bool bRequireUpdate = false;

	// IndexExtensions

	RString strIndexExtensions = m_eIndexExtensions.GetText();
	strIndexExtensions.Replace(_T(" "), _T(""));
	strIndexExtensions.Replace(_T(';'), _T('|'));
	strIndexExtensions.Trim(_T(";"));
	strIndexExtensions.MakeLower();

	if (strIndexExtensions != GETPREFSTR(_T("Database"), _T("IndexExtensions")))
	{
		SETPREFSTR(_T("Database"), _T("IndexExtensions"), strIndexExtensions);
		bRequireSync = true;
	}

	// IndexDirectories

	bool bIndexDirs = m_chkIndexDirectories.GetCheck();
	if (bIndexDirs != GETPREFBOOL(_T("Database"), _T("IndexDirectories")))
	{
		SETPREFBOOL(_T("Database"), _T("IndexDirectories"), bIndexDirs);
		bRequireSync = true;
	}

	// MaxInfoAge

	INT_PTR nMaxInfoAge = StringToNumber(m_eMaxInfoAge.GetText());
	if (nMaxInfoAge <= 0)
	{
		nMaxInfoAge = 2;
		m_eMaxInfoAge.SetText(_T("2"));
	}

	if (nMaxInfoAge != GETPREFINT(_T("Database"), _T("MaxInfoAge")))
	{
		SETPREFINT(_T("Database"), _T("MaxInfoAge"), (int)nMaxInfoAge);
		bRequireUpdate = true;
	}

	// Services
	
	RString str = m_cbOnlyUse.GetText(m_cbOnlyUse.GetSel());
	if (m_cbOnlyUse.GetSel() == m_cbOnlyUse.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("OnlyUse")))
		{SETPREFSTR(_T("InfoService"), _T("OnlyUse"), str); bRequireUpdate = true;}

	str = m_cbTitle.GetText(m_cbTitle.GetSel());
	if (m_cbTitle.GetSel() == m_cbTitle.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Title")))
		{SETPREFSTR(_T("InfoService"), _T("Title"), str); bRequireUpdate = true;}

	str = m_cbYear.GetText(m_cbYear.GetSel());
	if (m_cbYear.GetSel() == m_cbYear.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Year")))
		{SETPREFSTR(_T("InfoService"), _T("Year"), str); bRequireUpdate = true;}

	str = m_cbGenres.GetText(m_cbGenres.GetSel());
	if (m_cbGenres.GetSel() == m_cbGenres.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Genres")))
		{SETPREFSTR(_T("InfoService"), _T("Genres"), str); bRequireUpdate = true;}

	str = m_cbCountries.GetText(m_cbCountries.GetSel());
	if (m_cbCountries.GetSel() == m_cbCountries.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Countries")))
		{SETPREFSTR(_T("InfoService"), _T("Countries"), str); bRequireUpdate = true;}

	str = m_cbRuntime.GetText(m_cbRuntime.GetSel());
	if (m_cbRuntime.GetSel() == m_cbRuntime.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Runtime")))
		{SETPREFSTR(_T("InfoService"), _T("Runtime"), str); bRequireUpdate = true;}

	str = m_cbStoryline.GetText(m_cbStoryline.GetSel());
	if (m_cbStoryline.GetSel() == m_cbStoryline.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Storyline")))
		{SETPREFSTR(_T("InfoService"), _T("Storyline"), str); bRequireUpdate = true;}

	str = m_cbDirectors.GetText(m_cbDirectors.GetSel());
	if (m_cbDirectors.GetSel() == m_cbDirectors.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Directors")))
		{SETPREFSTR(_T("InfoService"), _T("Directors"), str); bRequireUpdate = true;}

	str = m_cbWriters.GetText(m_cbWriters.GetSel());
	if (m_cbWriters.GetSel() == m_cbWriters.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Writers")))
		{SETPREFSTR(_T("InfoService"), _T("Writers"), str); bRequireUpdate = true;}

	str = m_cbStars.GetText(m_cbStars.GetSel());
	if (m_cbStars.GetSel() == m_cbStars.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Stars")))
		{SETPREFSTR(_T("InfoService"), _T("Stars"), str); bRequireUpdate = true;}

	str = m_cbPoster.GetText(m_cbPoster.GetSel());
	if (m_cbPoster.GetSel() == m_cbPoster.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Poster")))
		{SETPREFSTR(_T("InfoService"), _T("Poster"), str); bRequireUpdate = true;}

	str = m_cbRating.GetText(m_cbRating.GetSel());
	if (m_cbRating.GetSel() == m_cbRating.GetCount()-1)
		str.Empty();
	if (str != GETPREFSTR(_T("InfoService"), _T("Rating")))
		{SETPREFSTR(_T("InfoService"), _T("Rating"), str); bRequireUpdate = true;}
	
	// Sync and update

	if (bRequireUpdate)
	{
		foreach (GetDB()->m_categories, cat)
			foreach (cat.directories, dir)
				foreach (dir.movies, mov)
					mov.bUpdated = false;
	}

	if (bRequireSync)
		GetDB()->SyncAndUpdate();
	else if (bRequireUpdate)
		GetDB()->Update();

	RPropertyPage::ApplyChanges();
}

bool CDatabasePage::OnCreate(CREATESTRUCT *pCS)
{
	if (!RPropertyPage::OnCreate(pCS))
		ASSERTRETURN(false);

	// Create controls

	m_bInitialized = false;

	DWORD cbStyle = CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|CBS_HASSTRINGS|WS_VSCROLL|WS_TABSTOP;

	if (
			!m_eIndexExtensions.Create<REdit>(m_hWnd, ES_AUTOHSCROLL|WS_TABSTOP, WS_EX_CLIENTEDGE) ||
			!m_stcIndexExtensions.Create<RStatic>(m_hWnd) ||
			!m_chkIndexDirectories.Create<RButton>(m_hWnd, BS_AUTOCHECKBOX|WS_TABSTOP) ||
			!m_eMaxInfoAge.Create<REdit>(m_hWnd, ES_AUTOHSCROLL|WS_TABSTOP|ES_NUMBER, WS_EX_CLIENTEDGE) ||
			!m_stcMaxInfoAge.Create<RStatic>(m_hWnd) ||
			!m_grpDatabase.Create<RButton>(m_hWnd, BS_GROUPBOX) ||

			!m_cbOnlyUse.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbTitle.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbYear.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbGenres.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbCountries.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbRuntime.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbStoryline.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbDirectors.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbWriters.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbStars.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbPoster.Create<RComboBox>(m_hWnd, cbStyle) ||
			!m_cbRating.Create<RComboBox>(m_hWnd, cbStyle) ||

			!m_stcOnlyUse.Create<RStatic>(m_hWnd) ||
			!m_stcTitle.Create<RStatic>(m_hWnd) ||
			!m_stcYear.Create<RStatic>(m_hWnd) ||
			!m_stcGenres.Create<RStatic>(m_hWnd) ||
			!m_stcCountries.Create<RStatic>(m_hWnd) ||
			!m_stcRuntime.Create<RStatic>(m_hWnd) ||
			!m_stcStoryline.Create<RStatic>(m_hWnd) ||
			!m_stcDirectors.Create<RStatic>(m_hWnd) ||
			!m_stcWriters.Create<RStatic>(m_hWnd) ||
			!m_stcStars.Create<RStatic>(m_hWnd) ||
			!m_stcPoster.Create<RStatic>(m_hWnd) ||
			!m_stcRating.Create<RStatic>(m_hWnd) ||
			!m_grpInfoService.Create<RButton>(m_hWnd, BS_GROUPBOX)
		)
		ASSERTRETURN(false);

	// Initialize controls

	RString strIndexExtensions = GETPREFSTR(_T("Database"), _T("IndexExtensions"));
	strIndexExtensions.Replace(_T('|'), _T(';'));
	m_eIndexExtensions.SetText(strIndexExtensions);

	m_chkIndexDirectories.SetCheck(GETPREFBOOL(_T("Database"), _T("IndexDirectories")));

	m_eMaxInfoAge.SetText(GETPREFSTR(_T("Database"), _T("MaxInfoAge")));


	// Populate services combo boxes

	RObArray<RString> services;
	services.Add(_T("imdb.com"));
	services.Add(_T("moviemeter.nl"));
	services.Add(_T(""));

	foreach (services, strServiceName)
	{
		m_cbOnlyUse.Add(strServiceName);
		m_cbTitle.Add(strServiceName);
		m_cbYear.Add(strServiceName);
		m_cbGenres.Add(strServiceName);
		m_cbCountries.Add(strServiceName);
		m_cbRuntime.Add(strServiceName);
		m_cbStoryline.Add(strServiceName);
		m_cbDirectors.Add(strServiceName);
		m_cbWriters.Add(strServiceName);
		m_cbStars.Add(strServiceName);
		m_cbPoster.Add(strServiceName);
		m_cbRating.Add(strServiceName);
	}

	m_cbWriters.Remove(1);

	// Select services according to preferences

	m_cbOnlyUse.SetSel(m_cbOnlyUse.GetCount()-1);
	m_cbTitle.SetSel(m_cbTitle.GetCount()-1);
	m_cbYear.SetSel(m_cbYear.GetCount()-1);
	m_cbGenres.SetSel(m_cbGenres.GetCount()-1);
	m_cbCountries.SetSel(m_cbCountries.GetCount()-1);
	m_cbRuntime.SetSel(m_cbRuntime.GetCount()-1);
	m_cbStoryline.SetSel(m_cbStoryline.GetCount()-1);
	m_cbDirectors.SetSel(m_cbDirectors.GetCount()-1);
	m_cbWriters.SetSel(m_cbWriters.GetCount()-1);
	m_cbStars.SetSel(m_cbStars.GetCount()-1);
	m_cbPoster.SetSel(m_cbPoster.GetCount()-1);
	m_cbRating.SetSel(m_cbRating.GetCount()-1);

	m_cbOnlyUse.SetSel(GETPREFSTR(_T("InfoService"), _T("OnlyUse")));
	m_cbTitle.SetSel(GETPREFSTR(_T("InfoService"), _T("Title")));
	m_cbYear.SetSel(GETPREFSTR(_T("InfoService"), _T("Year")));
	m_cbGenres.SetSel(GETPREFSTR(_T("InfoService"), _T("Genres")));
	m_cbCountries.SetSel(GETPREFSTR(_T("InfoService"), _T("Countries")));
	m_cbRuntime.SetSel(GETPREFSTR(_T("InfoService"), _T("Runtime")));
	m_cbStoryline.SetSel(GETPREFSTR(_T("InfoService"), _T("Storyline")));
	m_cbDirectors.SetSel(GETPREFSTR(_T("InfoService"), _T("Directors")));
	m_cbWriters.SetSel(GETPREFSTR(_T("InfoService"), _T("Writers")));
	m_cbStars.SetSel(GETPREFSTR(_T("InfoService"), _T("Stars")));
	m_cbPoster.SetSel(GETPREFSTR(_T("InfoService"), _T("Poster")));
	m_cbRating.SetSel(GETPREFSTR(_T("InfoService"), _T("Rating")));

	bool bCombined = (m_cbOnlyUse.GetSel() == m_cbOnlyUse.GetCount()-1);

	EnableWindow(m_cbTitle, bCombined);
	EnableWindow(m_cbYear, bCombined);
	EnableWindow(m_cbGenres, bCombined);
	EnableWindow(m_cbCountries, bCombined);
	EnableWindow(m_cbRuntime, bCombined);
	EnableWindow(m_cbStoryline, bCombined);
	EnableWindow(m_cbDirectors, bCombined);
	EnableWindow(m_cbWriters, bCombined);
	EnableWindow(m_cbStars, bCombined);
	EnableWindow(m_cbPoster, bCombined);
	EnableWindow(m_cbRating, bCombined);

	// Load control text

	OnPrefChanged();

	m_bInitialized = true;

	return true;
}

void CDatabasePage::OnSize(DWORD type, WORD cx, WORD cy)
{
	UNREFERENCED_PARAMETER(type);
	UNREFERENCED_PARAMETER(cy);
	int y = DUY(4);
	MoveWindow(m_grpDatabase, DUX(4), y, cx - DUX(8), DUY(70));
	y += DUY(12);
	MoveWindow(m_stcIndexExtensions, DUX(14), y, DUX(200), DUY(10));
	y += DUY(11);
	MoveWindow(m_eIndexExtensions, DUX(14), y, cx - DUX(28), DUY(12));
	y += DUY(15);
	MoveCheckBox(m_chkIndexDirectories, DUX(14), y);
	y += DUY(16);
	MoveStatic(m_stcMaxInfoAge, DUX(14), y+DUY(2));
	MoveWindow(m_eMaxInfoAge, DUX(14) + m_stcMaxInfoAge.GetWidth() + DUX(4), y, DUX(30), DUY(12));

	y = DUY(78);
	MoveWindow(m_grpInfoService, DUX(4), y, cx - DUX(8), DUY(112));
	y += DUY(12);
	MoveWindow(m_stcOnlyUse, DUX(14), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_cbOnlyUse, DUX(74), y, DUX(60), DUY(12));
	y += DUY(14);
	MoveWindow(m_stcTitle, DUX(14), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_cbTitle, DUX(74), y, DUX(60), DUY(12));
	MoveWindow(m_stcDirectors, DUX(156), y+DUY(1)+1, DUX(50), DUY(10));
	MoveWindow(m_cbDirectors, DUX(206), y, DUX(60), DUY(12));
	y += DUY(14);
	MoveWindow(m_stcYear, DUX(14), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_cbYear, DUX(74), y, DUX(60), DUY(12));
	MoveWindow(m_stcWriters, DUX(156), y+DUY(1)+1, DUX(50), DUY(10));
	MoveWindow(m_cbWriters, DUX(206), y, DUX(60), DUY(12));
	y += DUY(14);
	MoveWindow(m_stcGenres, DUX(14), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_cbGenres, DUX(74), y, DUX(60), DUY(12));
	MoveWindow(m_stcStars, DUX(156), y+DUY(1)+1, DUX(50), DUY(10));
	MoveWindow(m_cbStars, DUX(206), y, DUX(60), DUY(12));
	y += DUY(14);
	MoveWindow(m_stcCountries, DUX(14), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_cbCountries, DUX(74), y, DUX(60), DUY(12));
	MoveWindow(m_stcPoster, DUX(156), y+DUY(1)+1, DUX(50), DUY(10));
	MoveWindow(m_cbPoster, DUX(206), y, DUX(60), DUY(12));
	y += DUY(14);
	MoveWindow(m_stcRuntime, DUX(14), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_cbRuntime, DUX(74), y, DUX(60), DUY(12));
	MoveWindow(m_stcRating, DUX(156), y+DUY(1)+1, DUX(50), DUY(10));
	MoveWindow(m_cbRating, DUX(206), y, DUX(60), DUY(12));
	y += DUY(14);
	MoveWindow(m_stcStoryline, DUX(14), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_cbStoryline, DUX(74), y, DUX(60), DUY(12));

	PostMessage(m_hWnd, WM_PAINT);
	PostChildren(m_hWnd, WM_PAINT);
}

void CDatabasePage::OnPrefChanged()
{
	RPropertyPage::OnPrefChanged();

	m_stcIndexExtensions.SetText(GETSTR(IDS_INDEXEXTENSIONS) + _T(":"));
	m_chkIndexDirectories.SetText(_T(" ") + GETSTR(IDS_INDEXDIRECTORIES));
	m_stcMaxInfoAge.SetText(GETSTR(IDS_MAXINFOAGE) + _T(":"));
	m_grpDatabase.SetText(GETSTR(IDS_DATABASE));

	m_stcOnlyUse.SetText(GETSTR(IDS_ONLYUSE) + _T(":"));
	m_stcTitle.SetText(GETSTR(IDS_TITLE) + _T(":"));
	m_stcYear.SetText(GETSTR(IDS_YEAR) + _T(":"));
	m_stcGenres.SetText(GETSTR(IDS_GENRES) + _T(":"));
	m_stcCountries.SetText(GETSTR(IDS_COUNTRIES) + _T(":"));
	m_stcRuntime.SetText(GETSTR(IDS_RUNTIME) + _T(":"));
	m_stcStoryline.SetText(GETSTR(IDS_STORYLINE) + _T(":"));
	m_stcDirectors.SetText(GETSTR(IDS_DIRECTORS) + _T(":"));
	m_stcWriters.SetText(GETSTR(IDS_WRITERS) + _T(":"));
	m_stcStars.SetText(GETSTR(IDS_STARS) + _T(":"));
	m_stcPoster.SetText(GETSTR(IDS_POSTER) + _T(":"));
	m_stcRating.SetText(GETSTR(IDS_RATING) + _T(":"));
	m_cbOnlyUse.SetText(m_cbOnlyUse.GetCount()-1, GETSTR(IDS_COMBINED));
	m_cbTitle.SetText(m_cbTitle.GetCount()-1, GETSTR(IDS_NONE));
	m_cbYear.SetText(m_cbYear.GetCount()-1, GETSTR(IDS_NONE));
	m_cbGenres.SetText(m_cbGenres.GetCount()-1, GETSTR(IDS_NONE));
	m_cbCountries.SetText(m_cbCountries.GetCount()-1, GETSTR(IDS_NONE));
	m_cbRuntime.SetText(m_cbRuntime.GetCount()-1, GETSTR(IDS_NONE));
	m_cbStoryline.SetText(m_cbStoryline.GetCount()-1, GETSTR(IDS_NONE));
	m_cbDirectors.SetText(m_cbDirectors.GetCount()-1, GETSTR(IDS_NONE));
	m_cbWriters.SetText(m_cbWriters.GetCount()-1, GETSTR(IDS_NONE));
	m_cbStars.SetText(m_cbStars.GetCount()-1, GETSTR(IDS_NONE));
	m_cbPoster.SetText(m_cbPoster.GetCount()-1, GETSTR(IDS_NONE));
	m_cbRating.SetText(m_cbRating.GetCount()-1, GETSTR(IDS_NONE));
	m_grpInfoService.SetText(GETSTR(IDS_INFOSERVICE));

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
}

void CDatabasePage::OnDrawItem(UINT_PTR id, DRAWITEMSTRUCT *pDIS)
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

void CDatabasePage::OnMeasureItem(UINT_PTR id, MEASUREITEMSTRUCT *pMIS)
{
	RPropertyPage::OnMeasureItem(id, pMIS);
	pMIS->itemHeight = DUY(8);
}

void CDatabasePage::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	RPropertyPage::OnCommand(notifyCode, id, hWndControl);

	if (!m_bInitialized)
		return;

	if (notifyCode == CBN_SELENDOK || notifyCode == BN_CLICKED || notifyCode == EN_CHANGE)
	{
		SetChanged();

		if (hWndControl == m_cbOnlyUse)
		{
			bool bCombined = (m_cbOnlyUse.GetSel() == m_cbOnlyUse.GetCount()-1);

			EnableWindow(m_cbTitle, bCombined);
			EnableWindow(m_cbYear, bCombined);
			EnableWindow(m_cbGenres, bCombined);
			EnableWindow(m_cbCountries, bCombined);
			EnableWindow(m_cbRuntime, bCombined);
			EnableWindow(m_cbStoryline, bCombined);
			EnableWindow(m_cbDirectors, bCombined);
			EnableWindow(m_cbWriters, bCombined);
			EnableWindow(m_cbStars, bCombined);
			EnableWindow(m_cbPoster, bCombined);
			EnableWindow(m_cbRating, bCombined);

			if (!bCombined)
			{
				m_cbTitle.SetSel(m_cbTitle.GetCount()-1);
				m_cbYear.SetSel(m_cbYear.GetCount()-1);
				m_cbGenres.SetSel(m_cbGenres.GetCount()-1);
				m_cbCountries.SetSel(m_cbCountries.GetCount()-1);
				m_cbRuntime.SetSel(m_cbRuntime.GetCount()-1);
				m_cbStoryline.SetSel(m_cbStoryline.GetCount()-1);
				m_cbDirectors.SetSel(m_cbDirectors.GetCount()-1);
				m_cbWriters.SetSel(m_cbWriters.GetCount()-1);
				m_cbStars.SetSel(m_cbStars.GetCount()-1);
				m_cbPoster.SetSel(m_cbPoster.GetCount()-1);
				m_cbRating.SetSel(m_cbRating.GetCount()-1);

				RString strServiceName = m_cbOnlyUse.GetText(m_cbOnlyUse.GetSel());

				m_cbTitle.SetSel(strServiceName);
				m_cbYear.SetSel(strServiceName);
				m_cbGenres.SetSel(strServiceName);
				m_cbCountries.SetSel(strServiceName);
				m_cbRuntime.SetSel(strServiceName);
				m_cbStoryline.SetSel(strServiceName);
				m_cbDirectors.SetSel(strServiceName);
				m_cbWriters.SetSel(strServiceName);
				m_cbStars.SetSel(strServiceName);
				m_cbPoster.SetSel(strServiceName);
				m_cbRating.SetSel(strServiceName);
			}
		}
	}
}
