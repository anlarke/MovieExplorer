#ifndef __RSCROLLBAR_H__
#define __RSCROLLBAR_H__

// NOTE: Use the following code to obtain default width/height of the scroll bars
// int cx = GetSystemMetrics(SM_CXHTHUMB);
// int cy = GetSystemMetrics(SM_CYVTHUMB);

class RScrollBar : public RControl
{
	friend class RWindow;
	friend class RControl;

public:
	RScrollBar()
	{
	}

	~RScrollBar()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, bool bHorz)
	{
		if (!RControl::Create<DERIVED_TYPE>(WC_SCROLLBAR, hWndParent, WS_CHILD|WS_VISIBLE|
				WS_CLIPCHILDREN|WS_CLIPSIBLINGS|(bHorz ? SBS_HORZ : SBS_VERT)))
			ASSERTRETURN(false);

		return true;
	}

	int SetScrollInfo(int nRangeMin, int nRangeMax, int nPageSize = -1, int nPos = -1, bool bRedraw = true)
	{
		SCROLLINFO si;
		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE|(nPageSize != -1 ? SIF_PAGE : 0)|(nPos != -1 ? SIF_POS : 0);
		si.nMin = nRangeMin;
		si.nMax = nRangeMax;
		si.nPage = nPageSize;
		si.nPos = nPos;
		return (int)SendMessage(m_hWnd, SBM_SETSCROLLINFO, (WPARAM)bRedraw, (LPARAM)&si);
	}

	int SetPos(int nPos, bool bRedraw = true)
	{
		return (int)SendMessage(m_hWnd, SBM_SETPOS, nPos, (LPARAM)bRedraw);
	}

	int GetPos() const
	{
		return (int)SendMessage(m_hWnd, SBM_GETPOS, 0, 0);
	}

	int GetTrackPos() const
	{
		SCROLLINFO si;
		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		SendMessage(m_hWnd, SBM_GETSCROLLINFO, 0, (LPARAM)&si);
		return si.nTrackPos;
	}

	void GetRange(int& nMin, int& nMax)
	{
		SCROLLINFO si;
		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		SendMessage(m_hWnd, SBM_GETSCROLLINFO, 0, (LPARAM)&si);
		nMin = si.nMin;
		nMax = si.nMax;
	}
};

#endif // __RSCROLLBAR_H__
















/*
// ScrollBar.h : Defines the RScrollBar class.
//

// Use the following code to obtain default width/height of the scroll bars:
// int cx = GetSystemMetrics(SM_CXHTHUMB);
// int cy = GetSystemMetrics(SM_CYVTHUMB);

#pragma once

class RScrollBar
{
public:
	RScrollBar() : m_hWnd(NULL)
	{
	}

	virtual ~RScrollBar()
	{
		if (IsWindow(m_hWnd))
			DestroyWindow(m_hWnd);
	}

	bool Create(HWND hWndParent, bool bHorz)
	{
		m_hWnd = CreateWindow(_T("SCROLLBAR"), NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|
							  (bHorz ? SBS_HORZ : SBS_VERT), 0, 0, 0, 0, hWndParent, NULL, GetModuleHandle(NULL), NULL);
		
		if (!m_hWnd)
			ASSERTRETURN(false);

		return true;
	}

	operator HWND() const
	{
		return m_hWnd;
	}

	void Enable()
	{
		EnableWindow(m_hWnd, TRUE);
	}

	void Disable()
	{
		EnableWindow(m_hWnd, FALSE);
	}

	//int SetRange(int nMin, int nMax)
	//{
	//	return SendMessage(m_hWnd, SBM_SETRANGE, nMin, nMax);
	//}

	//void GetRange(int &nMin, int &nMax)
	//{
	//	SendMessage(m_hWnd, SBM_GETRANGE, (WPARAM)&nMin, (LPARAM)&nMax);
	//}

	int SetScrollInfo(int nRangeMin, int nRangeMax, int nPageSize, int nPos = -1, bool bRedraw = true)
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE|SIF_PAGE|(nPos != -1 ? SIF_POS : 0);
		si.nMin = nRangeMin;
		si.nMax = nRangeMax;
		si.nPage = nPageSize;
		si.nPos = nPos;
		return SendMessage(m_hWnd, SBM_SETSCROLLINFO, (WPARAM)bRedraw, (LPARAM)&si);
	}

	//bool GetScrollInfo(int &nRangeMin, int &nRangeMax, int &nPageSize, int &nPos, int &nTrackPos)
	//{
	//	SCROLLINFO si;
	//	si.cbSize = sizeof(SCROLLINFO);
	//	si.fMask = SIF_RANGE|SIF_PAGE|SIF_POS|SIF_TRACKPOS;
	//	bool bRet = SendMessage(m_hWnd, SBM_GETSCROLLINFO, 0, (LPARAM)&si);
	//	nRangeMin = si.nMin;
	//	nRangeMax = si.nMax;
	//	nPageSize = si.nPage;
	//	nPos = si.nPos;
	//	nTrackPos = si.nTrackPos;
	//}

	int SetPos(int nPos, bool bRedraw = true)
	{
		return SendMessage(m_hWnd, SBM_SETPOS, nPos, (LPARAM)bRedraw);
	}

	int GetPos()
	{
		return SendMessage(m_hWnd, SBM_GETPOS, 0, 0);
	}

protected:
	HWND m_hWnd;
};
*/