#include "stdafx.h"
#include "MovieExplorer.h"
#include "GridView.h"
#include "EditDlg.h"

#define LV_DETAILS_HEIGHT		330

CGridView::CGridView() : m_nHoverMov(-1), m_bScrolling(false), m_bCaptureM(false)
{
}

CGridView::~CGridView()
{
}

bool CGridView::OnCreate(CREATESTRUCT *pCS)
{
	if (!m_sb.Create<CScrollBar>(m_hWnd, false))
		ASSERTRETURN(false);

	m_mdc.Create(0, 0);
	OnPrefChanged();

	return true;
}

void CGridView::OnPaint(HDC hDC)
{
	m_mdc.Draw(hDC);
}

void CGridView::OnPrefChanged()
{
	// Get colors and fonts from preferences

	if (GETTHEMEBOOL(_T("ListView"), _T("BackgrWindowsDefault")))
	{
		m_clrBackgr = GetSysColor(COLOR_WINDOW);
		m_clrShadow = 0;
	}
	else
	{
		m_clrBackgr = GETTHEMECOLOR(_T("ListView"), _T("BackgrColor"));
		m_clrBackgrAlt = GETTHEMECOLOR(_T("ListView"), _T("BackgrAltColor"));
	}

	m_clrShadow = GETTHEMECOLOR(_T("ListView"), _T("ShadowColor"));
	m_aPosterShadow = GETTHEMEALPHA(_T("ListView"), _T("PosterShadowAlpha"));

	

	// Determine the services that are in use

	RString strPosterServ = GETPREFSTR(_T("InfoService"), _T("Poster"));

	m_servicesInUse.SetSize(0);
	if (m_servicesInUse.IndexOf(strPosterServ) == -1) m_servicesInUse.Add(strPosterServ);

	OnScaleChanged();
}

void CGridView::OnScaleChanged()
{

	// Create sprite for shadow behind poster

	m_sprShadow.Create(SCX(10), SCY(10), SCX(4), SCY(4), SCX(4), SCY(4));
	FillSolidRect(m_sprShadow, 0, 0, SCX(10), SCY(10), m_clrBackgr);
	DrawRoundedRect(m_sprShadow, 0, 0, SCX(10), SCY(10), SCX(4), m_clrShadow, m_aPosterShadow / 4, true);
	DrawRoundedRect(m_sprShadow, SCX(1), SCY(1), SCX(10) - 2 * SCX(1),
		SCY(10) - 2 * SCY(1), SCX(3), m_clrShadow, m_aPosterShadow / 2, true);
	DrawRoundedRect(m_sprShadow, 2 * SCX(1), 2 * SCY(1), SCX(10) - 4 * SCX(1),
		SCY(10) - 4 * SCY(1), SCX(2), m_clrShadow, m_aPosterShadow, true);


	// Calculate required column width

	/*HFONT hPrevFont = (HFONT)SelectObject(m_mdc, m_fntText);
	m_nColumnWidth = 0;
	m_nColumnWidth += SCX(10);
	SelectObject(m_mdc, hPrevFont);*/

	Draw();
}

bool CGridView::OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	foreach(m_links, link)
	{
		if (PtInRect(&link.rc, pt))
		{
			SetCursor(LoadCursor(NULL, IDC_HAND));
			return true;
		}
	}
	return RWindow::OnSetCursor(hWnd, hitTest, mouseMsg);
}

void CGridView::OnSize(DWORD type, WORD cx, WORD cy)
{
	VERIFY(m_mdc.Create(cx, cy));

	if (cx == 0 || cy == 0)
		return;

	Draw();
}

LRESULT CGridView::WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
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

void CGridView::OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
{
	switch (scrollCode)
	{
	case SB_PAGEDOWN:
		m_sb.SetPos(m_sb.GetPos() + SCY(30));
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

void CGridView::OnMouseWheel(WORD keys, short delta, short x, short y)
{
	UINT nScrollLines = 3;
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);

	int nPos = m_sb.GetPos();
	nPos -= round((float)(delta * (int)nScrollLines * SCY(30)) / WHEEL_DELTA);
	m_sb.SetPos(nPos);

	Draw();
}


void CGridView::Draw()
{
	// NOTE: The code in this function should be fast because it is called many times, the 
	// necessary preparation should have been performed in OnPrefChanged.

	int cx = m_mdc.cx;
	int cy = m_mdc.cy;
	INT_PTR nColumns = 4;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);

	// Draw background

	DrawRect(m_mdc, 0, 0, cx, cy, m_clrBackgr);

	// Calculate total list height and adjust scrollbar
	int nRows =  (int)ceil(((float)GetDB()->m_movies / (float)nColumns));
	int nHeight = (int)(nRows * SCY(LV_DETAILS_HEIGHT));

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

	//m_links.SetSize(0);
	//m_nHoverMov = -1;

	// Draw the movies

	//HFONT hPrevFont;
	RString str, strTitle, strDate;

	for (INT_PTR row = nStart; row < nRows && y < cy; row++)
	{

		for (INT_PTR col = 0; col < nColumns && (row * nColumns + col) < GetDB()->m_movies; col++)
		{
			//	RECT rcItem = { 0, y, cx, y + SCY(LV_DETAILS_HEIGHT) };

			//	if (i & 0x1)
			//		DrawRect(m_mdc, 0, y, cx, SCY(LV_DETAILS_HEIGHT), m_clrBackgrAlt);
			//DrawRect(m_mdc, 0, y - SCY(1), cx, SCY(1), m_clrShadow, m_aItemShadow);

			DBMOVIE& mov = *GetDB()->m_movies[row * nColumns + col];
			DBDIRECTORY& dir = *mov.pDirectory;
			DBCATEGORY& cat = *dir.pCategory;

			// draw poster

			if (mov.posterData.GetSize())
			{
				RMemoryDC mdcPoster;
				VERIFY(LoadImage(mov.posterData, mdcPoster, SCX(200), SCY(300)));
				int cxImg, cyImg;
				mdcPoster.GetDimensions(cxImg, cyImg);
				m_sprShadow.Draw(m_mdc, SCX(15) - 2 * SCX(1) + col * SCX(cxImg), y + SCY(15) - 2 * SCY(1),
					cxImg + 5 * SCX(1), cyImg + 5 * SCY(1));
				BitBlt(m_mdc, SCX(15) + col * SCX(cxImg), y + SCY(15), cxImg, cyImg, mdcPoster, 0, 0, SRCCOPY);
			}
			else
			{
				m_sprShadow.Draw(m_mdc, SCX(15) - 2 * SCX(1) + col * SCY(300), y + SCY(15) - 2 * SCY(1),
					SCX(200) + 5 * SCX(1), SCY(300) + 5 * SCY(1));
				FillSolidRect(m_mdc, SCX(15), y + SCY(15), SCX(200), SCX(300), m_clrBackgr);
			}


			// show edit button when mouse over item
			/*
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
			*/
		}
		y += SCY(LV_DETAILS_HEIGHT);
		

	}

	// Draw shadow line

	//DrawRect(m_mdc, 0, 0, cx, SCY(1), m_clrShadow, m_aRebarShadow);
	//DrawRect(m_mdc, 0, 0, cx, SCY(1), 0x000000, 100);
	//DrawRect(m_mdc, 0, SCY(1), cx, SCY(1), 0x000000, 50);
	//DrawRect(m_mdc, 0, 2*SCY(1), cx, SCY(1), 0x000000, 10);

	// Copy to screen

	Invalidate(m_hWnd);
	PostMessage(m_hWnd, WM_PAINT);
}


void CGridView::OnMouseMove(DWORD keys, short x, short y)
{
	bool bDraw = false;
	INT_PTR nColumns = 4;
	int nRows = (int)ceil(((float)GetDB()->m_movies / (float)nColumns));
	int nHeight = (int)(nRows * SCY(LV_DETAILS_HEIGHT));


	// Determine movie above which we're hovering

	RRect rcScrollBar;
	GetClientRectRelative(m_sb, m_hWnd, &rcScrollBar);
	//int cx = m_mdc.cx; // -rcScrollBar.cx;
	//int cy = m_mdc.cy;
	INT_PTR nStart = m_sb.GetPos() / SCY(LV_DETAILS_HEIGHT);
	//int yItem = (int)-(m_sb.GetPos() - nStart * SCY(LV_DETAILS_HEIGHT));
	INT_PTR nHoverMov = -1;

	int cxImg = 200, cyImg = 300;
	for (INT_PTR row = nStart; row < nRows ; row++)
	{

		for (INT_PTR col = 0; col < nColumns && (row * nColumns + col) < GetDB()->m_movies; col++)
		{
			//LOG(NumberToString(x) + _T(" ") + NumberToString(y) + _T("\n"));
			INT_PTR xx = SCX(15) + col * SCX(cxImg);
			INT_PTR yy = SCY(15) + row * SCY(cyImg);
			RECT rcItem = { xx, yy, xx+cxImg, yy+cyImg };
			/*LOG(NumberToString(SCX(15) + col * SCX(cxImg)) + _T(", ") + NumberToString(SCY(15) + row * SCY(cyImg)) +
				_T(", ") + NumberToString(cxImg) + _T(", ") + NumberToString(cyImg) + _T("\n"));*/
			if (PtInRect(&rcItem, x, y))
			{
				nHoverMov = row * nColumns + col; break;
			}
		}
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

void CGridView::OnLButtonDown(DWORD keys, short x, short y)
{

	RObArray<RString> keywords;
	SetFocus(m_hWnd);

	if (m_nHoverMov >= 0)
	{

		keywords.Add(GetDB()->m_movies[m_nHoverMov]->strFileName);
		GetDB()->FilterByKeywords(keywords);
		SendMessage(GetMainWnd(), WM_SWITCHVIEW);
	}
	//LOG(_T("Clicked on movie: ") + NumberToString(m_nHoverMov));
}