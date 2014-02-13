#include "stdafx.h"
#include "MovieExplorer.h"
#include "ListView.h"
#include "EditDlg.h"

#define LV_SMALL_STAR_SIZE		16
#define LV_LARGE_STAR_SIZE		24
#define LV_DETAILS_HEIGHT		330

#define LINKSTATE_NORMAL		0
#define LINKSTATE_HOVER			1

RString PrettyList(RString str)
{
	RString strResult;
	RArray<const TCHAR*> parts = SplitString(str, _T("|"), true);
	if (parts == 0)
		strResult = _T("");
	else if (parts == 1)
		strResult = parts[0];
	else if (parts == 2)
		strResult = (RString)parts[0] + _T(" ") + GETSTR(IDS_AND) + _T(" ") + parts[1];
	else
		strResult = (RString)parts[0] + _T(", ") + parts[1] + _T(" ") + 
				GETSTR(IDS_AND) + _T(" ") + parts[2];
	return strResult;
}

CListView::CListView() : m_nHoverMov(-1), m_bScrolling(false), m_bCaptureM(false)
{
}

CListView::~CListView()
{
}

void CListView::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	if (m_nHoverMov < 0 || m_nHoverMov >= GetDB()->m_movies)
		return;

	DBMOVIE& mov = *GetDB()->m_movies[m_nHoverMov];
	DBDIRECTORY& dir = *mov.pDirectory;
	DBCATEGORY& cat = *dir.pCategory;

	if (hWndControl == m_btnPlay)
	{
		RString strFilePath = CorrectPath(dir.strPath + _T("\\") + mov.strFileName, true);
		if (dir.strComputerName == GetComputerName())
		{
			// If it's a file open it. If it's a directory open the first video file inside
			// with a valid extension not containing 'sample'. Otherwise open the directory
			// so user can choose the file manually.

			if (FileExists(strFilePath))
				ShellExecute(HWND_DESKTOP, _T("open"), strFilePath, NULL, NULL, SW_SHOW);
			else if (DirectoryExists(strFilePath))
			{
				// Create easy to search list of extensions

				RString strIndexExtensions = GETPREFSTR(_T("Database"), _T("IndexExtensions"));
				strIndexExtensions = _T("|") + strIndexExtensions + _T("|");

				RString strPath = CorrectPath(strFilePath);
				RObArray<FILEINFO> fileInfos = EnumFiles(strPath, _T("*"));

				bool bFileFound = false;
				foreach (fileInfos, fi)
				{
					// See if extension is in the list of valid ones

					if (strIndexExtensions.FindNoCase(_T("|") + GetFileExt(fi.strName) + _T("|")) == -1)
						continue;

					// Make sure its not the 'sample' video

					if (fi.strName.FindNoCase(_T("sample")) >= 0)
						continue;

					RString strMoviePath = CorrectPath(strFilePath + _T("\\") + fi.strName);
					if (FileExists(strMoviePath))
					{
						ShellExecute(HWND_DESKTOP, _T("open"), strMoviePath, NULL, NULL, SW_SHOW);
						bFileFound = true;
						break;
					}
				}
				
				// We didn't find the movie in the directory so just open it

				if (!bFileFound)
					ShellExecute(HWND_DESKTOP, _T("open"), strFilePath, NULL, NULL, SW_SHOW);
			}
		}
		SetFocus(m_hWnd);
	}
	else if (hWndControl == m_btnDir)
	{
		if (dir.strComputerName == GetComputerName())
		{
			// If mov.strFileName points to a directory, open that directory. If it's really 
			// a file, open the directory the file is in.

			RString strDirPath = CorrectPath(dir.strPath);
			RString strFilePath = CorrectPath(dir.strPath + _T("\\") + mov.strFileName);

			if (DirectoryExists(strFilePath))
				ShellExecute(HWND_DESKTOP, _T("open"), strFilePath, NULL, NULL, SW_SHOW);
			else if (DirectoryExists(strDirPath))
				ShellExecute(HWND_DESKTOP, _T("open"), strDirPath, NULL, NULL, SW_SHOW);
		}
		SetFocus(m_hWnd);
	}
	else if (hWndControl == m_btnSeen)
	{
		m_btnSeen.SetCheck(!m_btnSeen.GetCheck());
		mov.bSeen = m_btnSeen.GetCheck();
		GetDB()->Filter();
		SetFocus(m_hWnd);
	}
	else if (hWndControl == m_btnEdit)
	{
		CEditDlg dlgEdit(GetMainWnd(), &mov);
		GetDB()->Filter();
		SetFocus(m_hWnd);
	}
	else if (hWndControl == m_btnHide)
	{
		if (m_btnHide.GetCheck() || MessageBox(GetMainWnd(), GETSTR(IDS_SURETOHIDE), 
				_T("Movie Explorer"), MB_ICONQUESTION|MB_YESNO) == IDYES)
		{
			m_btnHide.SetCheck(!m_btnHide.GetCheck());
			mov.bHide = m_btnHide.GetCheck();
			GetDB()->Filter();
		}
		SetFocus(m_hWnd);
	}
}

bool CListView::OnCreate(CREATESTRUCT *pCS)
{
	if (!m_sb.Create<CScrollBar>(m_hWnd, false))
		ASSERTRETURN(false);

	if (!m_btnPlay.Create<CToolBarButton>(m_hWnd, 0, &m_mdcPlayBtn) ||
			!m_btnDir.Create<CToolBarButton>(m_hWnd, 0, &m_mdcDirBtn) ||
			!m_btnSeen.Create<CToolBarButton>(m_hWnd, 0, &m_mdcSeenBtn) ||
			!m_btnEdit.Create<CToolBarButton>(m_hWnd, 0, &m_mdcEditBtn) ||
			!m_btnHide.Create<CToolBarButton>(m_hWnd, 0, &m_mdcHideBtn))
		ASSERTRETURN(false);

	m_mdc.Create(0, 0);
	OnPrefChanged();

	return true;
}

void CListView::OnLButtonDown(DWORD keys, short x, short y)
{
	SetFocus(m_hWnd);

	foreach (m_links, link)
	{
		if (PtInRect(&link.rc, x, y))
			ShellExecute(NULL, NULL, link.strURL, NULL, NULL, SW_SHOW);
	}
}

void CListView::OnMouseMove(DWORD keys, short x, short y)
{
	bool bDraw = false;

	foreach (m_links, link)
	{
		UINT_PTR state = PtInRect(&link.rc, x, y);
		if (link.state != state)
		{
			link.state = state;
			bDraw = true; 
			break;
		}
	}

	// Determine movie above which we're hovering

	RRect rcScrollBar;
	GetClientRectRelative(m_sb, m_hWnd, &rcScrollBar);
	int cx = m_mdc.cx - rcScrollBar.cx;
	int cy = m_mdc.cy;
	INT_PTR nStart = m_sb.GetPos() / SCY(LV_DETAILS_HEIGHT);
	int yItem = (int)-(m_sb.GetPos() - nStart * SCY(LV_DETAILS_HEIGHT));
	INT_PTR nHoverMov = -1;

	for (INT_PTR i = nStart; i < GetDB()->m_movies && yItem < cy; ++i, yItem += SCY(LV_DETAILS_HEIGHT))
	{
		RECT rcItem = {0, yItem, cx, yItem + SCY(LV_DETAILS_HEIGHT)};
		if (PtInRect(&rcItem, x, y))
			{nHoverMov = i; break;}
	}

	// Redraw if changed

	if (m_nHoverMov != nHoverMov)
	{
		m_nHoverMov = nHoverMov;
		bDraw = true;
	}

	// Redraw

	if (bDraw)
		Draw();

	if (!m_bCaptureM)
	{
		SetCaptureM(m_hWnd);
		m_bCaptureM = true;
	}
}

void CListView::OnMouseWheel(WORD keys, short delta, short x, short y)
{
	UINT nScrollLines = 3;
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);

	int nPos = m_sb.GetPos();
	nPos -= round((float)(delta * (int)nScrollLines * SCY(30)) / WHEEL_DELTA);
	m_sb.SetPos(nPos);

	Draw();
}

void CListView::OnRButtonDown(DWORD keys, short x, short y)
{
	SetFocus(m_hWnd);
}

void CListView::OnPaint(HDC hDC)
{
	m_mdc.Draw(hDC);
}

void CListView::OnPrefChanged()
{
	// Get colors and fonts from preferences

	if (GETTHEMEBOOL(_T("ListView"), _T("BackgrWindowsDefault")))
	{
		m_clrBackgr = GetSysColor(COLOR_WINDOW);
		m_clrShadow = 0;
		m_aRebarShadow = 0;
	}
	else
	{
		m_clrBackgr = GETTHEMECOLOR(_T("ListView"), _T("BackgrColor"));
		m_clrBackgrAlt = GETTHEMECOLOR(_T("ListView"), _T("BackgrAltColor"));
	}

	m_clrShadow = GETTHEMECOLOR(_T("ListView"), _T("ShadowColor"));
	m_aRebarShadow = GETTHEMEALPHA(_T("ListView"), _T("RebarShadowAlpha"));
	m_aItemShadow = GETTHEMEALPHA(_T("ListView"), _T("ItemShadowAlpha"));
	m_aPosterShadow = GETTHEMEALPHA(_T("ListView"), _T("PosterShadowAlpha"));

	m_bNormalizeRatings = GETPREFBOOL(_T("NormalizeRatings"));
	m_strRatingServ = GETPREFSTR(_T("InfoService"), _T("Rating"));

	m_clrTitle = GETTHEMECOLOR(_T("ListView"), _T("TitleFontColor"));
	m_clrText = GETTHEMECOLOR(_T("ListView"), _T("TextFontColor"));
	m_clrLink = GETTHEMECOLOR(_T("ListView"), _T("LinkColor"));

	// Create memory DCs for buttons

	m_mdcPlayBtn.Create(48, 48);
	DrawAlphaMap(m_mdcPlayBtn, 0, 0, IDA_PLAY, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	m_mdcDirBtn.Create(48, 48);
	DrawAlphaMap(m_mdcDirBtn, 0, 0, IDA_DIR, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	m_mdcSeenBtn.Create(48, 48);
	DrawAlphaMap(m_mdcSeenBtn, 0, 0, IDA_SEEN, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	m_mdcEditBtn.Create(48, 48);
	DrawAlphaMap(m_mdcEditBtn, 0, 0, IDA_EDIT, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	m_mdcHideBtn.Create(48, 48);
	DrawAlphaMap(m_mdcHideBtn, 0, 0, IDA_HIDE, 48, 48, 
			GETTHEMECOLOR(_T("ToolBarButton"), _T("IconColor")),
			GETTHEMEALPHA(_T("ToolBarButton"), _T("IconAlpha")));

	// Determine the services that are in use

	RString strTitleServ = GETPREFSTR(_T("InfoService"), _T("Title"));
	RString strYearServ = GETPREFSTR(_T("InfoService"), _T("Year"));
	RString strCountriesServ = GETPREFSTR(_T("InfoService"), _T("Countries"));
	RString strGenresServ = GETPREFSTR(_T("InfoService"), _T("Genres"));
	RString strRuntimeServ = GETPREFSTR(_T("InfoService"), _T("Runtime"));
	RString strStorylineServ = GETPREFSTR(_T("InfoService"), _T("Storyline"));
	RString strDirectorsServ = GETPREFSTR(_T("InfoService"), _T("Directors"));
	RString strWritersServ = GETPREFSTR(_T("InfoService"), _T("Writers"));
	RString strStarsServ = GETPREFSTR(_T("InfoService"), _T("Stars"));
	RString strPosterServ = GETPREFSTR(_T("InfoService"), _T("Poster"));
	RString strRatingServ = GETPREFSTR(_T("InfoService"), _T("Rating"));
	
	m_servicesInUse.SetSize(0);
	if (m_servicesInUse.IndexOf(strTitleServ) == -1) m_servicesInUse.Add(strTitleServ);
	if (m_servicesInUse.IndexOf(strYearServ) == -1) m_servicesInUse.Add(strYearServ);
	if (m_servicesInUse.IndexOf(strCountriesServ) == -1) m_servicesInUse.Add(strCountriesServ);
	if (m_servicesInUse.IndexOf(strGenresServ) == -1) m_servicesInUse.Add(strGenresServ);
	if (m_servicesInUse.IndexOf(strRuntimeServ) == -1) m_servicesInUse.Add(strRuntimeServ);
	if (m_servicesInUse.IndexOf(strStorylineServ) == -1) m_servicesInUse.Add(strStorylineServ);
	if (m_servicesInUse.IndexOf(strDirectorsServ) == -1) m_servicesInUse.Add(strDirectorsServ);
	if (m_servicesInUse.IndexOf(strWritersServ) == -1) m_servicesInUse.Add(strWritersServ);
	if (m_servicesInUse.IndexOf(strStarsServ) == -1) m_servicesInUse.Add(strStarsServ);
	if (m_servicesInUse.IndexOf(strPosterServ) == -1) m_servicesInUse.Add(strPosterServ);
	if (m_servicesInUse.IndexOf(strRatingServ) == -1) m_servicesInUse.Add(strRatingServ);

	OnScaleChanged();
}

void CListView::OnScaleChanged()
{
	// Create fonts
	
	GETTHEMEFONT(&m_fntTitle, _T("ListView"), _T("TitleFont"));
	GETTHEMEFONT(&m_fntText, _T("ListView"), _T("TextFont"));
	LOGFONT lf;
	GetObject(m_fntText, sizeof(LOGFONT), &lf);
	lf.lfWeight = FW_BOLD;
	VERIFY(m_fntTextBold.CreateIndirect(&lf));

	// Create sprite for shadow behind poster

	m_sprShadow.Create(SCX(10), SCY(10), SCX(4), SCY(4), SCX(4), SCY(4));
	FillSolidRect(m_sprShadow, 0, 0, SCX(10), SCY(10), m_clrBackgr);
	DrawRoundedRect(m_sprShadow, 0, 0, SCX(10), SCY(10), SCX(4), m_clrShadow, m_aPosterShadow / 4, true);
	DrawRoundedRect(m_sprShadow, SCX(1), SCY(1), SCX(10) - 2*SCX(1), 
			SCY(10) - 2*SCY(1), SCX(3), m_clrShadow, m_aPosterShadow / 2, true);
	DrawRoundedRect(m_sprShadow, 2*SCX(1), 2*SCY(1), SCX(10) - 4*SCX(1), 
			SCY(10) - 4*SCY(1), SCX(2), m_clrShadow, m_aPosterShadow, true);
		
	// Create memory DCs for star icons

	HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_STAR), 
			IMAGE_ICON, 64, 64, 0);
	RMemoryDC mdcStar(64, 64);
	DrawIconEx(mdcStar, 0, 0, hIcon, 64, 64, 0, NULL, DI_NORMAL);
	DestroyIcon(hIcon);

	RMemoryDC mdcTemp = mdcStar;
	Grayscale(&mdcTemp);
	Contrast(&mdcTemp, -52, true);
	RMemoryDC mdcStarEmpty(64, 64);
	AlphaBlt(mdcStarEmpty, 0, 0, 64, 64, mdcTemp, 0, 0, 64, 64, 50);

	m_mdcSmallStar.Create(SCX(LV_SMALL_STAR_SIZE), SCY(LV_SMALL_STAR_SIZE));
	Resize(&m_mdcSmallStar, &mdcStar, true);

	m_mdcSmallStarEmpty.Create(SCX(LV_SMALL_STAR_SIZE), SCY(LV_SMALL_STAR_SIZE));
	Resize(&m_mdcSmallStarEmpty, &mdcStarEmpty, true);

	m_mdcLargeStar.Create(SCX(LV_LARGE_STAR_SIZE), SCY(LV_LARGE_STAR_SIZE));
	Resize(&m_mdcLargeStar, &mdcStar, true);

	m_mdcLargeStarEmpty.Create(SCX(LV_LARGE_STAR_SIZE), SCY(LV_LARGE_STAR_SIZE));
	Resize(&m_mdcLargeStarEmpty, &mdcStarEmpty, true);

	// Calculate required column width

	HFONT hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
	m_nColumnWidth = 0;

	RObArray<RString> strings(4);
	strings[0] = GETSTR(IDS_DIRECTORS);
	strings[1] = GETSTR(IDS_STARS);
	strings[2] = GETSTR(IDS_CATEGORY);
	strings[3] = GETSTR(IDS_FILE);
	
	SIZE sz;
	foreach (strings, str)
	{
		GetTextExtentPoint32(m_mdc, str, (int)str.GetLength(), &sz);
		m_nColumnWidth = max(m_nColumnWidth, sz.cx);
	}

	m_nColumnWidth += SCX(10);
	SelectObject(m_mdc, hPrevFont);

	Draw();
}

bool CListView::OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	foreach (m_links, link)
	{
		if (PtInRect(&link.rc, pt))
		{
			SetCursor(LoadCursor(NULL, IDC_HAND));
			return true;
		}
	}
	return RWindow::OnSetCursor(hWnd, hitTest, mouseMsg);
}

void CListView::OnSize(DWORD type, WORD cx, WORD cy)
{
	VERIFY(m_mdc.Create(cx, cy));

	if (cx == 0 || cy == 0)
		return;

	Draw();
}

void CListView::OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
{
	switch (scrollCode)
	{
	case SB_LINEUP:
		m_sb.SetPos(m_sb.GetPos() - SCY(30));
		break;
	case SB_LINEDOWN:
		m_sb.SetPos(m_sb.GetPos() + SCY(30));
		break;
	case SB_THUMBTRACK:
		m_sb.SetPos(m_sb.GetTrackPos(), false);
		m_bScrolling = true;
		break;
	case SB_ENDSCROLL:
		m_bScrolling = false;
		break;
	}

	Draw();
}

LRESULT CListView::WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_DBUPDATED)
	{
		Draw();
		return 0;
	}
	else if (Msg == WM_CAPTUREM)
	{
		int x = (int)wParam;
		int y = (int)lParam;
		
		RECT rc, rcScrollBar;
		GetClientRect(m_hWnd, &rc);
		GetClientRect(m_sb, &rcScrollBar);
		rc.right -= rcScrollBar.right;

		if (!PtInRect(&rc, x, y))
		{
			if (m_bCaptureM)
			{
				ReleaseCaptureM(m_hWnd);
				m_bCaptureM = false;
				Draw();
			}
		}
	}
	return RWindow::WndProc(Msg, wParam, lParam);
}

void CListView::Draw()
{
	// NOTE: The code in this function should be fast because it is called many times, the 
	// necessary preparation should have been performed in OnPrefChanged.

	int cx = m_mdc.cx;
	int cy = m_mdc.cy;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);

	// Draw background

	DrawRect(m_mdc, 0, 0, cx, cy, m_clrBackgr);

	// Calculate total list height and adjust scrollbar

	int nHeight = (int)GetDB()->m_movies * SCY(LV_DETAILS_HEIGHT);

	if (nHeight > cy)
	{
		int cxThumb = GetSystemMetrics(SM_CXHTHUMB);
		MoveWindow(m_sb, cx - cxThumb, 0, cxThumb, cy);
		m_sb.SetScrollInfo(0, nHeight, cy);
		cx -= cxThumb;
	}
	else
	{
		MoveWindow(m_sb, 0, 0, 0, 0);
		m_sb.SetPos(0, false);
	}

	// Calculate index of first movie and the draw offset

	INT_PTR nStart = m_sb.GetPos() / SCY(LV_DETAILS_HEIGHT);
	int y = (int)-(m_sb.GetPos() - nStart * SCY(LV_DETAILS_HEIGHT));

	// Hide the buttons

	m_links.SetSize(0);
	MoveWindow(m_btnPlay, 0, 0, 0, 0);
	MoveWindow(m_btnDir, 0, 0, 0, 0);
	MoveWindow(m_btnSeen, 0, 0, 0, 0);
	MoveWindow(m_btnEdit, 0, 0, 0, 0);
	MoveWindow(m_btnHide, 0, 0, 0, 0);
	m_nHoverMov = -1;

	// Draw the movies

	HFONT hPrevFont;
	RString str;

	for (INT_PTR i = nStart; i < GetDB()->m_movies && y < cy; ++i, y += SCY(LV_DETAILS_HEIGHT))
	{
		RECT rcItem = {0, y, cx, y + SCY(LV_DETAILS_HEIGHT)};

		if (i & 0x1)
			DrawRect(m_mdc, 0, y, cx, SCY(LV_DETAILS_HEIGHT), m_clrBackgrAlt);
		DrawRect(m_mdc, 0, y - SCY(1), cx, SCY(1), m_clrShadow, m_aItemShadow);

		DBMOVIE& mov = *GetDB()->m_movies[i];
		DBDIRECTORY& dir = *mov.pDirectory;
		DBCATEGORY& cat = *dir.pCategory;

		// draw poster

		if (mov.posterData.GetSize())
		{
			RMemoryDC mdcPoster;
			VERIFY(LoadImage(mov.posterData, mdcPoster, SCX(200), SCY(300)));
			int cxImg, cyImg;
			mdcPoster.GetDimensions(cxImg, cyImg);
			m_sprShadow.Draw(m_mdc, SCX(15)-2*SCX(1), y + SCY(15)-2*SCY(1), 
					cxImg+5*SCX(1), cyImg+5*SCY(1));
			BitBlt(m_mdc, SCX(15), y + SCY(15), cxImg, cyImg, mdcPoster, 0, 0, SRCCOPY);
		}
		else
		{
			m_sprShadow.Draw(m_mdc, SCX(15)-2*SCX(1), y + SCY(15)-2*SCY(1), 
					SCX(200)+5*SCX(1), SCY(300)+5*SCY(1));
			FillSolidRect(m_mdc, SCX(15), y + SCY(15), SCX(200), SCX(300), m_clrBackgr);
		}

		// draw title and year

		hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTitle);
		SetTextColor(m_mdc, m_clrTitle);
		str = (mov.strTitle.IsEmpty() ? mov.strFileName : (mov.strTitle + 
				(mov.strYear.IsEmpty() ? _T("") : _T(" (") + mov.strYear + _T(")"))));
		TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(14), str);
		SelectObject(m_mdc, hPrevFont);

		// draw genres, countries and runtime

		mov.strCountries.Replace(_T("|"), _T("/"));
		mov.strGenres.Replace(_T("|"), _T("/"));
		mov.strRuntime.Replace(_T("|"), _T("/"));
		
		hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
		SetTextColor(m_mdc, m_clrText);
		TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(48), mov.strCountries);
		TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(66), mov.strGenres);
		TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(84), mov.strRuntime);
		SelectObject(m_mdc, hPrevFont);

		// draw storyline

		hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
		SetTextColor(m_mdc, m_clrText);
		RRect rc = RRect(SCX(200) + SCX(35), y + SCY(110), cx - SCX(15), 
				y + SCY(LV_DETAILS_HEIGHT) - SCY(92));
		str = mov.strStoryline;
		if (str.IsEmpty() && mov.strTitle.IsEmpty())
		{
			str = GETSTR(IDS_UPDATEINPROGRESS);
			foreach (m_servicesInUse, strServ)
			{
				RString& strID = (strServ == _T("imdb.com") ? mov.strIMDbID : mov.strMovieMeterID);
				if (strID == _T("unknown"))
					{str = GETSTR(IDS_MOVIEUNKNOWN); str.Replace(_T("%SERVICENAME%"), strServ); break;}
				else if (strID == _T("connError"))
					{str = GETSTR(IDS_CONNECTIONERROR); break;}
				else if (mov.strIMDbID == _T("scrapeError"))
					{str = GETSTR(IDS_SCRAPEERROR); break;}
			}
		}
		MLTextOut(m_mdc, rc.left, rc.top, rc.Width(), rc.Height(), str, SCY(18));
		SelectObject(m_mdc, hPrevFont);

		// draw directors

		if (!mov.strDirectors.IsEmpty())
		{
			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTextBold);
			SetTextColor(m_mdc, m_clrText);
			TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(LV_DETAILS_HEIGHT) - SCY(91), 
					GETSTR(IDS_DIRECTORS) + _T(":"));
			SelectObject(m_mdc, hPrevFont);

			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
			SetTextColor(m_mdc, m_clrText);
			TextOut(m_mdc, SCX(200) + SCX(35) + m_nColumnWidth + SCX(10), 
					y + SCY(LV_DETAILS_HEIGHT) - SCY(91), PrettyList(mov.strDirectors));
			SelectObject(m_mdc, hPrevFont);
		}

		// draw writers

		if (!mov.strWriters.IsEmpty())
		{
			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
			SetTextColor(m_mdc, m_clrText);
			SIZE sz;
			GetTextExtentPoint32(m_mdc, PrettyList(mov.strDirectors), &sz);
			int nOffset = sz.cx;
			SelectObject(m_mdc, hPrevFont);

			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTextBold);
			SetTextColor(m_mdc, m_clrText);
			TextOut(m_mdc, SCX(200) + SCX(35) + m_nColumnWidth + SCX(10) + sz.cx + SCX(30), 
					y + SCY(LV_DETAILS_HEIGHT) - SCY(91), GETSTR(IDS_WRITERS) + _T(":"));
			GetTextExtentPoint32(m_mdc, GETSTR(IDS_WRITERS), &sz);
			nOffset += sz.cx;
			SelectObject(m_mdc, hPrevFont);

			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
			SetTextColor(m_mdc, m_clrText);
			TextOut(m_mdc, SCX(200) + SCX(35) + m_nColumnWidth + SCX(10) + nOffset + SCX(46), 
					y + SCY(LV_DETAILS_HEIGHT) - SCY(91), PrettyList(mov.strWriters));
			SelectObject(m_mdc, hPrevFont);
		}
		
		// draw stars

		if (!mov.strStars.IsEmpty())
		{
			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTextBold);
			SetTextColor(m_mdc, m_clrText);
			TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(LV_DETAILS_HEIGHT) - SCY(72), 
					GETSTR(IDS_STARS) + _T(":"));
			SelectObject(m_mdc, hPrevFont);

			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
			SetTextColor(m_mdc, m_clrText);
			TextOut(m_mdc, SCX(200) + SCX(35) + m_nColumnWidth + SCX(10), 
					y + SCY(LV_DETAILS_HEIGHT) - SCY(72), PrettyList(mov.strStars));
			SelectObject(m_mdc, hPrevFont);
		}

		// draw category (and directory)

		hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTextBold);
		SetTextColor(m_mdc, m_clrText);
		TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(LV_DETAILS_HEIGHT) - SCY(52), 
				GETSTR(IDS_CATEGORY) + _T(":"));
		SelectObject(m_mdc, hPrevFont);

		hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
		SetTextColor(m_mdc, m_clrText);
		TextOut(m_mdc, SCX(200) + SCX(35) + m_nColumnWidth + SCX(10), 
				y + SCY(LV_DETAILS_HEIGHT) - SCY(52), cat.strName + _T(" ["));
		SIZE sz;
		GetTextExtentPoint32(m_mdc, cat.strName + _T(" ["), &sz);
		TextOut(m_mdc, SCX(200) + SCX(35) + m_nColumnWidth + SCX(10) + sz.cx, 
				y + SCY(LV_DETAILS_HEIGHT) - SCY(52), dir.strPath + _T("]"));
		SelectObject(m_mdc, hPrevFont);

		// draw file/entry

		hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTextBold);
		SetTextColor(m_mdc, m_clrText);
		TextOut(m_mdc, SCX(200) + SCX(35), y + SCY(LV_DETAILS_HEIGHT) - SCY(34), 
				GETSTR(IDS_FILE) + _T(":"));
		SelectObject(m_mdc, hPrevFont);

		hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
		SetTextColor(m_mdc, m_clrText);
		TextOut(m_mdc, SCX(200) + SCX(35) + m_nColumnWidth + SCX(10),
				y + SCY(LV_DETAILS_HEIGHT) - SCY(34), mov.strFileName + 
				(mov.fileSize != 0 ? _T(" [") + SizeToString(mov.fileSize) + 
				_T("]") : _T("")));
		SelectObject(m_mdc, hPrevFont);

		// draw rating

		if (mov.fRating != 0.0f)
		{
			float fRating = mov.fRating;
			float fRatingMax = mov.fRatingMax;

			if (m_bNormalizeRatings)
			{
				fRating *= 10.0f / fRatingMax;
				fRatingMax = 10.0f;
			}

			int nRating = (int)fRating;
			int nRatingMax = (int)fRatingMax;
			int j;

			if (nRatingMax <= 5)
			{
				int nLargeStarSize = SCX(LV_LARGE_STAR_SIZE);
				int cxStarPart = (int)((fRating - floor(fRating)) * 
						SCX(LV_LARGE_STAR_SIZE));

				for (j = 0; j < nRating; j++)
					AlphaBlt(m_mdc, cx - (nRatingMax - j) * (nLargeStarSize + SCX(2)) - SCX(10), y + SCY(14), 
						nLargeStarSize, nLargeStarSize, m_mdcLargeStar);
					
				AlphaBlt(m_mdc, cx - (nRatingMax - nRating) * (nLargeStarSize + SCX(2)) - SCX(10), y + SCY(14),
						cxStarPart, nLargeStarSize, m_mdcLargeStar);

				AlphaBlt(m_mdc, cx - (nRatingMax - nRating) * (nLargeStarSize + SCX(2)) - SCX(10) + cxStarPart, y + SCY(14),
						nLargeStarSize - cxStarPart, nLargeStarSize, m_mdcLargeStarEmpty, cxStarPart);

				for (j = nRating + 1; j < nRatingMax; j++)
					AlphaBlt(m_mdc, cx - (nRatingMax - j) * (nLargeStarSize + SCX(2)) - SCX(10), y + SCY(14),
							nLargeStarSize, nLargeStarSize, m_mdcLargeStarEmpty);
			}
			else
			{
				int nSmallStarSize = SCX(LV_SMALL_STAR_SIZE);
				int cxStarPart = (int)((fRating - floor(fRating)) * nSmallStarSize);

				for (j = 0; j < nRating; j++)
					AlphaBlt(m_mdc, cx - (nRatingMax - j) * (nSmallStarSize + SCX(1)) - SCX(10), y + SCY(20),
							nSmallStarSize, nSmallStarSize, m_mdcSmallStar);

				AlphaBlt(m_mdc, cx - (nRatingMax - nRating) * (nSmallStarSize + SCX(1)) - SCX(10), y + SCY(20),
						cxStarPart, nSmallStarSize, m_mdcSmallStar);

				AlphaBlt(m_mdc, cx - (nRatingMax - nRating) * (nSmallStarSize + SCX(1)) - SCX(10) + cxStarPart, y + SCY(20),
						nSmallStarSize - cxStarPart, nSmallStarSize, m_mdcSmallStarEmpty, cxStarPart);

				for (j = nRating + 1; j < nRatingMax; j++)
					AlphaBlt(m_mdc, cx - (nRatingMax - j) * (nSmallStarSize + SCX(1)) - SCX(10), y + SCY(20),
							nSmallStarSize, nSmallStarSize, m_mdcSmallStarEmpty);
			}

			// draw rating text of active service (right aligned)

			RString str = _T(" (") + NumberToString(mov.nVotes) + _T(" ") + 
					GETSTR(IDS_VOTES) + _T(")");
			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
			SetTextColor(m_mdc, m_clrText);
			GetTextExtentPoint32(m_mdc, str, &sz);
			int x = cx - sz.cx - SCX(15);
			TextOut(m_mdc, x, y + SCY(48), str);
			SelectObject(m_mdc, hPrevFont);

			if (fRatingMax <= 5.0f)
				_stprintf(str.GetBuffer(32), _T("%.2f"), fRating);
			else
				_stprintf(str.GetBuffer(32), _T("%.1f"), fRating);
			str.ReleaseBuffer();
			str.Replace(_T("."), GetDecimalSep());
			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTextBold);
			GetTextExtentPoint32(m_mdc, str, &sz);
			x -= sz.cx;
			SetTextColor(m_mdc, m_clrText);
			TextOut(m_mdc, x, y + SCY(48), str);
			SelectObject(m_mdc, hPrevFont);

			str = _T(": ");
			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
			GetTextExtentPoint32(m_mdc, str, &sz);
			SetTextColor(m_mdc, m_clrText);
			x -= sz.cx;
			TextOut(m_mdc, x, y + SCY(48), str);
			SelectObject(m_mdc, hPrevFont);

			LINK *pLink = m_links.AddNew();
			if (m_strRatingServ == _T("imdb.com"))
			{
				pLink->strText = _T("IMDb");
				pLink->strURL = _T("http://www.imdb.com/title/") + mov.strIMDbID + _T("/");
			}
			else if (m_strRatingServ == _T("moviemeter.nl"))
			{
				pLink->strText = _T("MovieMeter");
				pLink->strURL = _T("http://www.moviemeter.nl/film/") + mov.strMovieMeterID;
			}
			else
				ASSERT(false);
			hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
			GetTextExtentPoint32(m_mdc, pLink->strText , &sz);
			pLink->rc.x = x - sz.cx;
			pLink->rc.cx = sz.cx;
			pLink->rc.y = y + SCY(48);
			pLink->rc.cy = sz.cy;
			pLink->state = (PtInRect(&pLink->rc, pt) ? LINKSTATE_HOVER : LINKSTATE_NORMAL);
			SetTextColor(m_mdc, (pLink->state == LINKSTATE_HOVER ? m_clrLink : m_clrText));
			TextOut(m_mdc, pLink->rc.x, pLink->rc.y, pLink->strText);
			SelectObject(m_mdc, hPrevFont);

			// Draw metascore if active service is IMDB

			if (mov.nMetascore >= 0 && m_strRatingServ == _T("imdb.com"))
			{
				LINK *pMetascoreLink = m_links.AddNew();
				pMetascoreLink->strText = _T("Metascore: ") + NumberToString(mov.nMetascore) + _T("/100");
				pMetascoreLink->strURL = _T("http://www.imdb.com/title/") + mov.strIMDbID + _T("/criticreviews");
				hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
				GetTextExtentPoint32(m_mdc, pMetascoreLink->strText, &sz);
				pMetascoreLink->rc.x = cx - sz.cx - SCX(15);
				pMetascoreLink->rc.cx = sz.cx;
				pMetascoreLink->rc.y = y + SCY(66);
				pMetascoreLink->rc.cy = sz.cy;
				pMetascoreLink->state = (PtInRect(&pMetascoreLink->rc, pt) ? LINKSTATE_HOVER : LINKSTATE_NORMAL);
				SetTextColor(m_mdc, (pMetascoreLink->state == LINKSTATE_HOVER ? m_clrLink : m_clrText));
				TextOut(m_mdc, pMetascoreLink->rc.x, pMetascoreLink->rc.y, pMetascoreLink->strText);
				SelectObject(m_mdc, hPrevFont);
			}
			else if (mov.fIMDbRating != 0.0f && m_strRatingServ != _T("imdb.com"))
			{
				// Draw IMDb rating if active service is not IMDb

				RString str = _T(" (") + NumberToString(mov.nIMDbVotes) + _T(" ") + 
						GETSTR(IDS_VOTES) + _T(")");
				hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
				SetTextColor(m_mdc, m_clrText);
				GetTextExtentPoint32(m_mdc, str, &sz);
				int x = cx - sz.cx - SCX(15);
				TextOut(m_mdc, x, y + SCY(66), str);
				SelectObject(m_mdc, hPrevFont);
			
				_stprintf(str.GetBuffer(32), _T("%.1f"), mov.fIMDbRating);
				str.ReleaseBuffer();
				str.Replace(_T("."), GetDecimalSep());
				hPrevFont = (HFONT)SelectObject(m_mdc, m_fntTextBold);
				GetTextExtentPoint32(m_mdc, str, &sz);
				x -= sz.cx;
				SetTextColor(m_mdc, m_clrText);
				TextOut(m_mdc, x, y + SCY(66), str);
				SelectObject(m_mdc, hPrevFont);

				str = _T(": ");
				hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
				GetTextExtentPoint32(m_mdc, str, &sz);
				SetTextColor(m_mdc, m_clrText);
				x -= sz.cx;
				TextOut(m_mdc, x, y + SCY(66), str);
				SelectObject(m_mdc, hPrevFont);

				LINK *pLink = m_links.AddNew();
				pLink->strText = _T("IMDb");
				pLink->strURL = _T("http://www.imdb.com/title/") + mov.strIMDbID + _T("/");
				hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
				GetTextExtentPoint32(m_mdc, pLink->strText , &sz);
				pLink->rc.x = x - sz.cx;
				pLink->rc.cx = sz.cx;
				pLink->rc.y = y + SCY(66);
				pLink->rc.cy = sz.cy;
				pLink->state = (PtInRect(&pLink->rc, pt) ? LINKSTATE_HOVER : LINKSTATE_NORMAL);
				SetTextColor(m_mdc, (pLink->state == LINKSTATE_HOVER ? m_clrLink : m_clrText));
				TextOut(m_mdc, pLink->rc.x, pLink->rc.y, pLink->strText);
				SelectObject(m_mdc, hPrevFont);
			}
		}

		// show edit button when mouse over item

		if (!m_bScrolling && PtInRect(&rcItem, pt) && pt.y >= 0)
		{
			m_nHoverMov = i;

			RRect rcBtn;
			rcBtn.left = cx - SCX(154);
			rcBtn.top = y + SCY(LV_DETAILS_HEIGHT) - SCX(42);
			rcBtn.right = cx;
			rcBtn.bottom = y + SCY(LV_DETAILS_HEIGHT);
			DrawRect(m_mdc, &rcBtn, (i & 0x1 ? m_clrBackgrAlt : m_clrBackgr));
			
			rcBtn.cx = SCX(24);
			rcBtn.x = cx - SCX(15) - rcBtn.cx;
			rcBtn.cy = SCX(24);
			rcBtn.y = y + SCY(LV_DETAILS_HEIGHT) - SCY(15) - rcBtn.cy;
			
			MoveWindow(m_btnHide, &rcBtn);
			rcBtn.x -= rcBtn.cx + SCX(4);
			MoveWindow(m_btnEdit, &rcBtn);
			rcBtn.x -= rcBtn.cx + SCX(4);
			MoveWindow(m_btnSeen, &rcBtn);

			RString strCorrDirPath = CorrectPath(dir.strPath, true);
			if ((dir.strComputerName == GetComputerName() || dir.strComputerName.IsEmpty()) &&
					(DirectoryExists(strCorrDirPath) || FileExists(strCorrDirPath)))
			{
				rcBtn.x -= rcBtn.cx + SCX(4);
				MoveWindow(m_btnDir, &rcBtn);
			}

			RString strCorrFilePath = CorrectPath(dir.strPath + _T("\\") + mov.strFileName, true);
			if ((dir.strComputerName == GetComputerName() || dir.strComputerName.IsEmpty()) &&
					(DirectoryExists(strCorrFilePath) || FileExists(strCorrFilePath)))
			{
				rcBtn.x -= rcBtn.cx + SCX(4);
				MoveWindow(m_btnPlay, &rcBtn);
			}

			m_btnSeen.SetCheck(mov.bSeen);
			m_btnHide.SetCheck(mov.bHide);

			//PostChildren(m_hWnd, WM_PAINT);
		}
	}

	// Draw shadow line

	DrawRect(m_mdc, 0, 0, cx, SCY(1), m_clrShadow, m_aRebarShadow);
	//DrawRect(m_mdc, 0, 0, cx, SCY(1), 0x000000, 100);
	//DrawRect(m_mdc, 0, SCY(1), cx, SCY(1), 0x000000, 50);
	//DrawRect(m_mdc, 0, 2*SCY(1), cx, SCY(1), 0x000000, 10);

	// Copy to screen

	Invalidate(m_hWnd);
	PostMessage(m_hWnd, WM_PAINT);
}
