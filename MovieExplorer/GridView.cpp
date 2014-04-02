#include "stdafx.h"
#include "MovieExplorer.h"
#include "GridView.h"

#define GV_DETAILS_HEIGHT		330
#define GV_POSTER_HEIGHT		300
#define GV_PAGESIZE				300
#define GV_LINESIZE				30

CGridView::CGridView() : m_nHoverMov(-1), m_bScrolling(false), m_bCaptureM(false), m_nColumns(4)
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
	m_aRebarShadow = GETTHEMEALPHA(_T("ListView"), _T("RebarShadowAlpha"));
	m_clrText = GETTHEMECOLOR(_T("ListView"), _T("TextFontColor"));

	// Determine the services that are in use

	RString strPosterServ = GETPREFSTR(_T("InfoService"), _T("Poster"));

	m_servicesInUse.SetSize(0);
	if (m_servicesInUse.IndexOf(strPosterServ) == -1) m_servicesInUse.Add(strPosterServ);

	OnScaleChanged();
}

void CGridView::OnScaleChanged()
{

	GETTHEMEFONT(&m_fntText, _T("ListView"), _T("TextFont"));

	// Create sprite for shadow behind poster

	m_sprShadow.Create(SCX(10), SCY(10), SCX(4), SCY(4), SCX(4), SCY(4));
	FillSolidRect(m_sprShadow, 0, 0, SCX(10), SCY(10), m_clrBackgr);
	DrawRoundedRect(m_sprShadow, 0, 0, SCX(10), SCY(10), SCX(4), m_clrShadow, m_aPosterShadow / 4, true);
	DrawRoundedRect(m_sprShadow, SCX(1), SCY(1), SCX(10) - 2 * SCX(1),
		SCY(10) - 2 * SCY(1), SCX(3), m_clrShadow, m_aPosterShadow / 2, true);
	DrawRoundedRect(m_sprShadow, 2 * SCX(1), 2 * SCY(1), SCX(10) - 4 * SCX(1),
		SCY(10) - 4 * SCY(1), SCX(2), m_clrShadow, m_aPosterShadow, true);


	// Calculate required column width

	m_nColumns = (m_mdc.cx - SCX(30)) / SCX(200);

	Draw();
}

bool CGridView::OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	return RWindow::OnSetCursor(hWnd, hitTest, mouseMsg);
}

void CGridView::OnSize(DWORD type, WORD cx, WORD cy)
{
	VERIFY(m_mdc.Create(cx, cy));

	if (cx == 0 || cy == 0)
		return;

	m_nColumns = (m_mdc.cx - SCX(30)) / SCX(200);

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
	case SB_PAGEUP:
		m_sb.SetPos(m_sb.GetPos() - SCY(GV_PAGESIZE));
		break;
	case SB_LINEUP:
		m_sb.SetPos(m_sb.GetPos() - SCY(GV_LINESIZE));
		break;
	case SB_PAGEDOWN:
		m_sb.SetPos(m_sb.GetPos() + SCY(GV_PAGESIZE));
		break;
	case SB_LINEDOWN:
		m_sb.SetPos(m_sb.GetPos() + SCY(GV_LINESIZE));
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

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);

	// Draw background

	DrawRect(m_mdc, 0, 0, cx, cy, m_clrBackgr);

	// Calculate total list height and adjust scrollbar
	int nRows =  (int)ceil(((float)GetDB()->m_movies / (float)m_nColumns));
	int nHeight = (int)(nRows * SCY(GV_POSTER_HEIGHT));

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

	INT_PTR nStart = m_sb.GetPos() / SCY(GV_POSTER_HEIGHT);
	int y = (int)-(m_sb.GetPos() - nStart * SCY(GV_POSTER_HEIGHT));

	// Draw the movies

	//HFONT hPrevFont;
	RString str, strTitle, strDate;

	for (INT_PTR row = nStart; row < nRows && y < cy; row++)
	{

		for (INT_PTR col = 0; col < m_nColumns && (row * m_nColumns + col) < GetDB()->m_movies; col++)
		{
			DBMOVIE& mov = *GetDB()->m_movies[row * m_nColumns + col];
			DBDIRECTORY& dir = *mov.pDirectory;
			DBCATEGORY& cat = *dir.pCategory;

			// draw poster

			if (mov.posterData.GetSize())
			{
				RMemoryDC mdcPoster;
				VERIFY(LoadImage(mov.posterData, mdcPoster, SCX(200), SCY(300)));
				int cxImg, cyImg;
				mdcPoster.GetDimensions(cxImg, cyImg);
				m_sprShadow.Draw(m_mdc, SCX(15) - 2 * SCX(1) + col * cxImg, y + SCY(15) - 2 * SCY(1),
					cxImg + 5 * SCX(1), cyImg + 5 * SCY(1));
				BitBlt(m_mdc, SCX(15) + col * cxImg, y + SCY(15), cxImg, cyImg, mdcPoster, 0, 0, SRCCOPY);
			}
			else
			{
				m_sprShadow.Draw(m_mdc, SCX(15) - 2 * SCX(1) + col * SCX(200), y + SCY(15) - 2 * SCY(1),
					SCX(200) + 5 * SCX(1), SCY(300) + 5 * SCY(1));
				FillSolidRect(m_mdc, SCX(15) + col * SCX(200), y + SCY(15), SCX(200), SCY(300), m_clrBackgr);

				// Write filename centered in poster

				SIZE sz; 
				SelectObject(m_mdc, m_fntText);
				SetTextColor(m_mdc, m_clrText);
				GetTextExtentPoint32(m_mdc, mov.strFileName, &sz);
				RECT rc = { SCX(15) + col * SCX(200), y + SCY(15), SCX(15) + col * SCX(200) + SCX(200), y + SCY(15) + SCY(300) };
				
				if (!mov.strTitle.IsEmpty())
					DrawText(m_mdc, mov.strTitle, &rc, DT_CENTER | DT_WORDBREAK);
				else
					DrawText(m_mdc, mov.strFileName, &rc, DT_CENTER | DT_WORDBREAK);
			}
		}
		y += SCY(GV_POSTER_HEIGHT);
	}

	// Draw shadow line

	DrawRect(m_mdc, 0, 0, cx, SCY(1), m_clrShadow, m_aRebarShadow);
	
	// Copy to screen

	Invalidate(m_hWnd);
	PostMessage(m_hWnd, WM_PAINT);
}


void CGridView::OnMouseMove(DWORD keys, short x, short y)
{
	// Determine movie above which we're hovering

	bool bDraw = false;
	int nRows = (int)ceil(((float)GetDB()->m_movies / (float)m_nColumns));
	int nHeight = (int)(nRows * SCY(GV_POSTER_HEIGHT));
	INT_PTR nStart = m_sb.GetPos() / SCY(GV_POSTER_HEIGHT);
	int nYRow = (int)-(m_sb.GetPos() - nStart * SCY(GV_POSTER_HEIGHT));
	INT_PTR nHoverMov = -1;


	int cxImg = 200, cyImg = 300;
	for (INT_PTR row = nStart; row < nRows ; row++)
	{

		for (INT_PTR col = 0; col < m_nColumns && (row * m_nColumns + col) < GetDB()->m_movies; col++)
		{
			INT_PTR nX = SCX(15) + col * SCX(cxImg);
			INT_PTR nY = nYRow + SCY(15);
			RECT rcItem = { nX, nY, nX+SCX(cxImg), nY+SCY(cyImg) };
			if (PtInRect(&rcItem, x, y))
			{
				nHoverMov = row * m_nColumns + col; break;
			}
		}
		nYRow += SCY(GV_POSTER_HEIGHT);
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

void CGridView::OnKeyDown(UINT virtKey, WORD repCount, UINT flags)
{
	switch (virtKey)
	{

	case VK_PRIOR:
		// page up
		m_sb.SetPos(m_sb.GetPos() - SCY(GV_PAGESIZE));
		break;
	case VK_UP:
	case VK_NUMPAD8:
		// move it up
		m_sb.SetPos(m_sb.GetPos() - SCY(GV_LINESIZE));
		break;
	case VK_DOWN:
	case VK_NUMPAD2:
		// move it down
		m_sb.SetPos(m_sb.GetPos() + SCY(GV_LINESIZE));
		break;
	case VK_NEXT:
		//page down
		m_sb.SetPos(m_sb.GetPos() + SCY(GV_PAGESIZE));
		break;
	}

	Draw();

}


void CGridView::OnLButtonDown(DWORD keys, short x, short y)
{

	RObArray<RString> keywords;
	SetFocus(m_hWnd);

	if (m_nHoverMov >= 0)
	{

		//keywords.Add(GetDB()->m_movies[m_nHoverMov]->strFileName);
		//GetDB()->FilterByKeywords(keywords);
		SendMessage(GetMainWnd(), WM_LISTVIEW_ITEM, m_nHoverMov);
	}
	//LOG(_T("Clicked on movie: ") + NumberToString(m_nHoverMov));
}