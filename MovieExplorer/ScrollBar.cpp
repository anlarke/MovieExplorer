#include "stdafx.h"
#include "MovieExplorer.h"
#include "ScrollBar.h"

#define TMR_SCROLLUPWAIT	1
#define TMR_SCROLLUP		2
#define TMR_SCROLLDOWNWAIT	3
#define TMR_SCROLLDOWN		4

CScrollBar::CScrollBar() : m_bHorz(false), m_bMouseOver(false), m_bWindowsDefault(false), 
		m_bDragging(false), m_nRangeMin(0), m_nRangeMax(0), m_nPageSize(0), m_nPos(0), 
		m_nTrackPos(0), m_nHighlight(-1), m_nPressed(-1), m_bTimer(false)
{
}

CScrollBar::~CScrollBar()
{
}

int CScrollBar::SetScrollInfo(int nRangeMin, int nRangeMax, 
		int nPageSize /*= -1*/, int nPos /*= -1*/)
{
	if (m_bWindowsDefault)
		return m_sb.SetScrollInfo(nRangeMin, nRangeMax, nPageSize, nPos);

	// Assign new values

	m_nRangeMin = nRangeMin;
	m_nRangeMax = nRangeMax;
	if (nPageSize != -1)
		m_nPageSize = nPageSize;
	if (nPos != -1)
		m_nPos = nPos;

	// Correct positions

	if (m_nPos < nRangeMin)
		m_nPos = nRangeMin;
	if (m_nPos > nRangeMax - m_nPageSize)
		m_nPos = nRangeMax - m_nPageSize;

	if (m_nTrackPos < nRangeMin)
		m_nTrackPos = nRangeMin;
	if (m_nTrackPos > nRangeMax - m_nPageSize)
		m_nTrackPos = nRangeMax - m_nPageSize;

	// Redraw

	bool bDragging = m_bDragging; // TODO: find better solution
	m_bDragging = false;
	CalcRects();
	m_bDragging = bDragging;

	Draw();

	return m_nPos;
}

void CScrollBar::SetPos(int nPos, bool bRedraw)
{
	if (m_bWindowsDefault)
		{m_sb.SetPos(nPos, bRedraw); return;}

	if (nPos < m_nRangeMin)
		nPos = m_nRangeMin;
	if (nPos > m_nRangeMax - m_nPageSize)
		nPos = m_nRangeMax - m_nPageSize;

	m_nPos = m_nTrackPos = nPos;

	if (!m_bDragging)
		CalcRects();

	if (bRedraw)
		Draw();
}

int CScrollBar::GetPos() const
{
	return (m_bWindowsDefault ? m_sb.GetPos() : m_nPos);
}

int CScrollBar::GetTrackPos() const
{
	return (m_bWindowsDefault ? m_sb.GetTrackPos() : m_nTrackPos);
}

bool CScrollBar::OnCreate(CREATESTRUCT *pCS)
{
	if (!m_sb.Create<RScrollBar>(m_hWnd, m_bHorz))
		{ASSERT(false); return false;}
	PrepareDraw();
	return true;
}

void CScrollBar::OnLButtonDblClk(DWORD keys, short x, short y)
{
	OnLButtonDown(keys, x, y);
}

void CScrollBar::OnLButtonDown(DWORD keys, short x, short y)
{
	if (PtInRect(&m_rcBtn1, x, y))
	{
		PostMessage(GetParent(m_hWnd), WM_VSCROLL, MAKEWPARAM(SB_LINEUP, m_nPos), (LPARAM)m_hWnd);
		SetTimer(m_hWnd, SB_LINEUP, 300, NULL);
		m_bTimer = true;
	}
	else if (PtInRect(&m_rcThumb, x, y))
	{
		m_bDragging = true;
		m_ptDragging.x = x;
		m_ptDragging.y = y;
		m_rcDragging = m_rcThumb;
		m_nHighlight = -1;
		m_nPressed = 1;
	}
	else if (PtInRect(&m_rcBtn2, x, y))
	{
		PostMessage(GetParent(m_hWnd), WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, m_nPos), (LPARAM)m_hWnd);
		SetTimer(m_hWnd, SB_LINEDOWN, 300, NULL);
		m_bTimer = true;
	}
}

void CScrollBar::OnLButtonUp(DWORD keys, short x, short y)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		m_nPressed = -1;
		ZeroMemory(&m_ptDragging, sizeof(POINT));
		CalcRects();
		OnMouseMove(keys, x, y);
		Draw();
		PostMessage(GetParent(m_hWnd), WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL, m_nPos), (LPARAM)m_hWnd);
	}
	KillTimer(m_hWnd, SB_LINEUP);
	KillTimer(m_hWnd, SB_LINEDOWN);
	m_bTimer = false;
}

void CScrollBar::OnMouseMove(DWORD keys, short x, short y)
{
	if (m_bDragging)
	{
		m_bMouseOver = true;
		CalcRects(y);
		Draw();
	}
	else
	{
		bool bDraw = false;

		// Redraw if mouse enters or leaves control

		RECT rc;
		GetClientRect(m_hWnd, &rc);
		bool bMouseOver = (bool)PtInRect(&rc, x, y);
		if (m_bMouseOver != bMouseOver)
		{
			m_bMouseOver = bMouseOver;
			bDraw = true;
		}

		// Manage button states

		RRect *ppRects[3] = {&m_rcBtn1, &m_rcThumb, &m_rcBtn2};
		for (INT_PTR i = 0; i < 3; ++i)
		{
			if (PtInRect(ppRects[i], x, y))
			{
				if (m_nPressed != i)
				{
					m_nHighlight = i;
					bDraw = true;
				}
			}
			else
			{
				if (m_nHighlight == i)
				{
					m_nHighlight = -1;
					bDraw = true;
				}
			}
		}

		// We might need to redraw

		if (bDraw)
			Draw();
	}

	// Set capture if over window, otherwise release it

	if (m_bMouseOver || m_bTimer)
		SetCapture(m_hWnd);
	else
		ReleaseCapture();
}

void CScrollBar::OnPaint(HDC hDC)
{
	if (m_bWindowsDefault)
		return;

	m_mdc.Draw(hDC);
}

void CScrollBar::OnPrefChanged()
{
	PrepareDraw();
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
}

void CScrollBar::OnSize(DWORD type, WORD cx, WORD cy)
{
	if (m_bWindowsDefault)
	{
		MoveWindow(m_sb, 0, 0, cx, cy);
		return;
	}

	int cxPrev = m_mdc.cx;
	int cyPrev = m_mdc.cy;

	VERIFY(m_mdc.Create(cx, cy));

	if ((m_bHorz && cy != cyPrev) || (!m_bHorz && cx != cxPrev))
		PrepareDraw();

	CalcRects();
	Draw();
}

void CScrollBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == SB_LINEUP)
	{
		PostMessage(GetParent(m_hWnd), WM_VSCROLL, MAKEWPARAM(SB_LINEUP, m_nPos), (LPARAM)m_hWnd);
		SetTimer(m_hWnd, SB_LINEUP, 50, NULL);
	}
	else if (nIDEvent == SB_LINEDOWN)
	{
		PostMessage(GetParent(m_hWnd), WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, m_nPos), (LPARAM)m_hWnd);
		SetTimer(m_hWnd, SB_LINEDOWN, 50, NULL);
	}
}

void CScrollBar::OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
{
	PostMessage(GetParent(m_hWnd), WM_VSCROLL, MAKEWPARAM(scrollCode, pos), (LPARAM)m_hWnd);
}

void CScrollBar::CalcRects(int y /*= 0*/)
{
	const int &cx = m_mdc.cx;
	const int &cy = m_mdc.cy;

	const int cxBtn = GetSystemMetrics(SM_CXHTHUMB);
	const int cyBtn = cxBtn + SCX(1);

	// Buttons

	m_rcBtn1 = RRect(0, 0, cxBtn, cyBtn);
	m_rcBtn2 = RRect(0, cy - cyBtn, cxBtn, cy);

	// Thumb

	m_rcThumb.x = 0;
	m_rcThumb.cx = cxBtn;
	
	int nFullRange = m_nRangeMax - m_nRangeMin + 1;
	int nVarRange = nFullRange - m_nPageSize;

	if (nVarRange <= 0)
		return;

	int cyFullRange = cy - 2 * cyBtn;
	m_rcThumb.cy = max((m_nPageSize * cyFullRange) / nFullRange, SCX(20));
	int cyVarRange = cyFullRange - m_rcThumb.cy;

	if (cyVarRange <= 0)
		return;

	if (m_bDragging)
	{
		// Calculate virtual thumb position from mouse position and correct it

		m_rcThumb = m_rcDragging;
		m_rcThumb.y += y - m_ptDragging.y;

		if (m_rcThumb.y < cyBtn)
			m_rcThumb.y = cyBtn;

		if (m_rcThumb.bottom > cy - cyBtn)
			m_rcThumb.y -= m_rcThumb.bottom - (cy - cyBtn);

		// Calculate track position from virtual thumb position

		double dVarPos = (double)(m_rcThumb.y - cyBtn) / cyVarRange;
		int nTrackPos = m_nRangeMin + round(dVarPos * nVarRange);
		
		if (m_nTrackPos != nTrackPos)
		{
			m_nTrackPos = nTrackPos;
			PostMessage(GetParent(m_hWnd), WM_VSCROLL, 
					MAKEWPARAM(SB_THUMBTRACK, m_nTrackPos), (LPARAM)m_hWnd);
		}

		// Calculate thumb position from track position

		m_rcThumb.y = cyBtn + round((double)((m_nTrackPos - m_nRangeMin) * 
				cyVarRange) / nVarRange);
	}
	else
	{
		m_rcThumb.y = cyBtn + round((double)((m_nPos - m_nRangeMin) * 
				cyVarRange) / nVarRange);
	}
}

void CScrollBar::Draw()
{
	if (m_bWindowsDefault)
		return;

	if (m_bHorz)
		{ASSERT(false); return;} // TODO

	const int &cx = m_mdc.cx;
	const int &cy = m_mdc.cy;

	m_sprBackgr.Draw(m_mdc, 0, 0, cx, cy, true);

	RECT drawRects[3] = {m_rcBtn1, m_rcThumb, m_rcBtn2};
	InflateRect(&drawRects[0], 0, -SCX(1));
	InflateRect(&drawRects[2], 0, -SCX(1));

	for (INT_PTR i = 0; i < 3; ++i)
	{
		if (!m_bMouseOver && (i == 0 || i == 2))
			continue;
		(m_nPressed == i ? m_sprThumbPressed : (m_nHighlight == i ? m_sprThumbHighlight : 
			m_sprThumbNormal)).Draw(m_mdc, &drawRects[i], true);
	}

	if (m_rcThumb.cy > SCX(18))
		m_mdcGripper.Draw(m_mdc, SCX(4), m_rcThumb.y + m_rcThumb.cy / 2 - SCX(4), true);

	(m_bMouseOver ? m_mdcArrowUpActive : 
			m_mdcArrowUpInactive).Draw(m_mdc, SCX(4), SCX(7), true);
	(m_bMouseOver ? m_mdcArrowDownActive : 
			m_mdcArrowDownInactive).Draw(m_mdc, SCX(4), cy - SCX(10), true);

	Invalidate(m_hWnd);
	SendMessage(m_hWnd, WM_PAINT);
}

void CScrollBar::PrepareDraw()
{
	m_bWindowsDefault = GETTHEMEBOOL(_T("ScrollBar"), _T("BackgrWindowsDefault"));
	if (m_bWindowsDefault)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		MoveWindow(m_sb, &rc);
		return;
	}

	MoveWindow(m_sb, 0, 0, 0, 0);

	if (m_bHorz)
	{
		ASSERT(false); // TODO
	}
	else
	{
		int cxBtn = GetSystemMetrics(SM_CXHTHUMB);
		int cyBtn = cxBtn + SCX(1);

		// Create background sprite

		m_sprBackgr.Create(cxBtn, cyBtn, SCX(1), SCX(1), SCX(1), SCX(1));
		DrawRect(m_sprBackgr, 0, 0, cxBtn, cyBtn, 
				GETTHEMECOLOR(_T("ScrollBar"), _T("BackgrEdgeColor")));
		DrawRect(m_sprBackgr, SCX(1), 0, cxBtn - 2 * SCX(1), cyBtn - SCX(1), 
				GETTHEMECOLOR(_T("ScrollBar"), _T("BackgrColor")));

		// Create thumb/button sprite

		int r = GETTHEMEINT(_T("ScrollBar"), _T("ThumbRadius"));
		if (r < 0)
			r = 0;
		if (r > 4)
			r = 4;

		int cyThumb = 2 * SCX(r+1) + 1;

		m_sprThumbNormal.Create(cxBtn, cyThumb, SCX(r+1), SCX(r+1), SCX(r+1), SCX(r+1));
		DrawRoundedRectGradientH(m_sprThumbNormal, SCX(1), 0, cxBtn - 2 * SCX(1), cyThumb, SCX(r), 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbNormalColorL")),
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbNormalColorR")));
		DrawRoundedRect(m_sprThumbNormal, SCX(1), 0, cxBtn - 2 * SCX(1), cyThumb, SCX(r), 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbNormalEdge1Color")),
				GETTHEMEALPHA(_T("ScrollBar"), _T("ThumbNormalEdge1Alpha")), false);
		DrawRoundedRect(m_sprThumbNormal, SCX(2), SCX(1), cxBtn - 2 * SCX(2), 
				cyThumb - 2 * SCX(1), SCX(r > 0 ? r-1 : 0), 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbNormalEdge2Color")),
				GETTHEMEALPHA(_T("ScrollBar"), _T("ThumbNormalEdge2Alpha")), false);

		m_sprThumbHighlight.Create(cxBtn, cyThumb, SCX(r), SCX(r), SCX(r), SCX(r));
		DrawRoundedRectGradientH(m_sprThumbHighlight, SCX(1), 0, cxBtn - 2 * SCX(1), 
				cyThumb, SCX(r), 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbHighlightColorL")),
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbHighlightColorR")));
		DrawRoundedRect(m_sprThumbHighlight, SCX(1), 0, cxBtn - 2 * SCX(1), cyThumb, SCX(r), 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbHighlightEdge1Color")),
				GETTHEMEALPHA(_T("ScrollBar"), _T("ThumbHighlightEdge1Alpha")), false);
		DrawRoundedRect(m_sprThumbHighlight, SCX(2), SCX(1), cxBtn - 2 * SCX(2), 
				cyThumb - 2 * SCX(1), SCX(r > 0 ? r-1 : 0), 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ThumbHighlightEdge2Color")),
				GETTHEMEALPHA(_T("ScrollBar"), _T("ThumbHighlightEdge2Alpha")), false);

		m_sprThumbPressed = m_sprThumbHighlight;

		// Create arrow sprites

		RMemoryDC mdcTemp(18, 8);
		DrawAlphaMap(mdcTemp, 0, 0, IDA_SBARROWUP, 18, 8, 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ArrowActiveColor")),
				GETTHEMEALPHA(_T("ScrollBar"), _T("ArrowActiveAlpha")));
		m_mdcArrowUpActive.Create(SCX(9), SCX(4));
		Resize(&m_mdcArrowUpActive, &mdcTemp, true);
		m_mdcArrowDownActive = Flip(m_mdcArrowUpActive);

		mdcTemp.Create(18, 8);
		DrawAlphaMap(mdcTemp, 0, 0, IDA_SBARROWUP, 18, 8, 
				GETTHEMECOLOR(_T("ScrollBar"), _T("ArrowInactiveColor")),
				GETTHEMEALPHA(_T("ScrollBar"), _T("ArrowInactiveAlpha")));
		m_mdcArrowUpInactive.Create(SCX(9), SCX(4));
		Resize(&m_mdcArrowUpInactive, &mdcTemp, true);
		m_mdcArrowDownInactive = Flip(m_mdcArrowUpInactive);

		// Create gripper sprite

		m_mdcGripper.Create(cxBtn - SCX(7), SCX(9));
		for (int y = 0; y < m_mdcGripper.cy; y += SCX(3))
		{
			DrawRectGradientH(m_mdcGripper, 0, y, m_mdcGripper.cx - SCX(1), SCX(2),
					GETTHEMECOLOR(_T("ScrollBar"), _T("GripperColorL")),
					GETTHEMECOLOR(_T("ScrollBar"), _T("GripperColorR")),
					GETTHEMEALPHA(_T("ScrollBar"), _T("GripperAlphaL")),
					GETTHEMEALPHA(_T("ScrollBar"), _T("GripperAlphaR")));
			DrawRect(m_mdcGripper, m_mdcGripper.cx - SCX(1), y, SCX(1), SCX(3), 
					GETTHEMECOLOR(_T("ScrollBar"), _T("GripperEdgeColor")),
					GETTHEMEALPHA(_T("ScrollBar"), _T("GripperEdgeAlpha")));
			DrawRect(m_mdcGripper, 0, y + SCX(2), m_mdcGripper.cx - SCX(1), SCX(1), 
					GETTHEMECOLOR(_T("ScrollBar"), _T("GripperEdgeColor")),
					GETTHEMEALPHA(_T("ScrollBar"), _T("GripperEdgeAlpha")));
		}
	}
}
