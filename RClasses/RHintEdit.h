#ifndef __RHINTEDIT_H__
#define __RHINTEDIT_H__

//
// REMEMBER TO REFLECT THE OnCtlColorEdit MESSAGE TO THIS CONTROL FROM THE PARENT WINDOW!
//

class RHintEdit : public REdit
{
	friend class RControl;
	friend class RWindow;

public:
	RHintEdit() : m_clrNormal(0), m_clrHint(0), m_clrBackgr(0), 
			m_bShowHint(false), m_bUpdate(false), m_hbrBackgr(NULL)
	{
	}

	~RHintEdit()
	{
	}

	void SetHint(const TCHAR *lpszHint)
	{
		m_strHint = lpszHint;
		Update();
	}

	void SetHintColor(COLORREF clr)
	{
		m_clrHint = clr;
		Update();
	}

	void SetTextColor(COLORREF clr)
	{
		m_clrNormal = clr;
		Update();
	}

	void SetBackgrColor(COLORREF clr)
	{
		m_clrBackgr = clr;
		DeleteObject(m_hbrBackgr);
		m_hbrBackgr = CreateSolidBrush(m_clrBackgr);
		Update();
	}

	HBRUSH ReflectCtlColorEdit(HDC hDC, HWND hWnd)
	{
		ASSERT(hWnd == m_hWnd);
		::SetTextColor(hDC, (m_bShowHint ? m_clrHint : m_clrNormal));
		SetBkColor(hDC, m_clrBackgr);
		//SetBkMode(hDC, TRANSPARENT); // does not play nice with ES_MULTILINE style
		return m_hbrBackgr;
	}

	RString GetText()
	{
		RString str;
		if (!m_bShowHint)
			str = REdit::GetText();
		return str;
	}

protected:
	bool OnCreate(CREATESTRUCT *pCS)
	{
		if (!REdit::OnCreate(pCS))
			return false;

		m_clrNormal = GetSysColor(COLOR_WINDOWTEXT);
		m_clrHint = GetSysColor(COLOR_BTNSHADOW);
		m_clrBackgr = GetSysColor(COLOR_WINDOW);
		m_hbrBackgr = CreateSolidBrush(m_clrBackgr);
		m_bShowHint = (GetWindowTextLength(m_hWnd) == 0);
		m_bUpdate = true;

		return true;
	}

	void OnDestroy()
	{
		DeleteObject(m_hbrBackgr);
	}

	void OnKillFocus(HWND hWndGetFocus)
	{
		REdit::OnKillFocus(hWndGetFocus);
		if (GetWindowTextLength(m_hWnd) == 0)
		{
			m_bShowHint = true;
			Update();
		}
	}

	void OnSetFocus(HWND hWndLoseFocus)
	{
		REdit::OnSetFocus(hWndLoseFocus);
		if (m_bShowHint)
		{
			m_bShowHint = false;
			REdit::SetText(NULL);
			Update();
		}
	}

	void OnSetFont(HFONT hFont, bool bRedraw)
	{
		REdit::OnSetFont(hFont, bRedraw);

		if (m_bUpdate)
		{
			LOGFONT lf;
			GetObject(hFont, sizeof(LOGFONT), &lf);
			VERIFY(m_fntNormal.CreateIndirect(&lf));
			lf.lfItalic = TRUE;
			VERIFY(m_fntHint.CreateIndirect(&lf));
			Update();
		}
	}

	LRESULT OnSetText(const TCHAR *lpszText)
	{
		LRESULT res = REdit::OnSetText(lpszText);
		if (m_bUpdate && GetFocus() != m_hWnd)
		{
			m_bShowHint = (!lpszText || _tcslen(lpszText) == 0);
			Update();
		}
		return res;
	}

	void Update()
	{
		if (m_bShowHint)
		{
			m_bUpdate = false;
			SetFont(m_fntHint);
			SetText(m_strHint);
			m_bUpdate = true;
		}
		else
		{
			m_bUpdate = false;
			SetFont(m_fntNormal);
			m_bUpdate = true;
		}
	}

	RFont m_fntNormal, m_fntHint;
	COLORREF m_clrNormal, m_clrHint, m_clrBackgr;
	RString m_strHint;
	bool m_bShowHint, m_bUpdate;
	HBRUSH m_hbrBackgr;
};

#endif // __RHINTEDIT_H__









/*
class RHintEdit : public REdit
{
public:
	RHintEdit() : m_lpfnPrevWndProc(NULL), m_hbrBackgr(NULL)
	{
	}

	~RHintEdit()
	{
	}
	
	bool Create(HWND hWndParent, const TCHAR* lpszText = NULL, const TCHAR* lpszHint = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0)
	{
		if (IsWindow(m_hWnd))
		{
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}

		// Create edit control as usual

		m_hWnd = CreateWindowEx(dwExStyle, WC_EDIT, lpszText, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|dwStyle, 
							    0, 0, 0, 0, hWndParent, NULL, GetModuleHandle(NULL), NULL);
		
		if (!m_hWnd)
			ASSERTRETURN(false);
		
		// Subclass window
		
		m_lpfnPrevWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG)(WNDPROC)WndProc);
		SetProp(m_hWnd, _T("pControl"), (HANDLE)this);

		// Set properties

		m_clrNormal = GetSysColor(COLOR_WINDOWTEXT);
		m_clrHint = HE_HINTCOLOR;
		m_clrBackgr = GetSysColor(COLOR_WINDOW);
		m_hbrBackgr = CreateSolidBrush(m_clrBackgr);
		m_bEmpty = (GetWindowTextLength(m_hWnd) == 0);

		SetFont(GetDialogFont());
		SetHint(lpszHint);

		return true;
	}

	void SetHint(const TCHAR* lpszHint)
	{
		ASSERT(IsWindow(m_hWnd));
		m_strHint = lpszHint;
		Hint();
	}

	void SetTextColor(COLORREF clr)
	{
		ASSERT(IsWindow(m_hWnd));
		m_clrNormal = clr;
		InvalidateRect(m_hWnd, NULL, FALSE);
	}

	void SetHintColor(COLORREF clr)
	{
		ASSERT(IsWindow(m_hWnd));
		m_clrHint = clr;
		InvalidateRect(m_hWnd, NULL, FALSE);
	}

	void SetBackgrColor(COLORREF clr)
	{
		DeleteObject(m_hbrBackgr);
		m_clrBackgr = clr;
		m_hbrBackgr = CreateSolidBrush(m_clrBackgr);
		InvalidateRect(m_hWnd, NULL, FALSE);
	}

	virtual HBRUSH OnCtlColorEdit(HDC hDC, HWND hWnd)
	{
		if (GetFocus() != m_hWnd && m_bEmpty)
			::SetTextColor(hDC, m_clrHint);
		else
			::SetTextColor(hDC, m_clrNormal);
		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, m_clrBackgr);
		return m_hbrBackgr;
	}

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		RHintEdit *pControl = (RHintEdit*)GetProp(hWnd, _T("pControl"));
		ASSERT(pControl);

		LOGFONT lf;
		LRESULT res;

		switch (Msg)
		{
		case WM_NCDESTROY:
			DeleteObject(pControl->m_hbrBackgr);
			pControl->m_hbrBackgr = NULL;
			SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pControl->m_lpfnPrevWndProc);
			RemoveProp(hWnd, _T("pControl"));
			break;
		case WM_SETFONT:
			ASSERT(wParam);
			GetObject((HFONT)wParam, sizeof(LOGFONT), &lf);
			VERIFY(pControl->m_fntNormal.CreateIndirect(&lf));
			lf.lfItalic = TRUE;
			VERIFY(pControl->m_fntHint.CreateIndirect(&lf));
			res = CallWindowProc(pControl->m_lpfnPrevWndProc, hWnd, Msg, wParam, lParam);
			pControl->Hint();
			return res;
		case WM_SETFOCUS:
			pControl->Hint();
			break;
		case WM_KILLFOCUS:
			pControl->m_bEmpty = (GetWindowTextLength(pControl->m_hWnd) == 0);
			pControl->Hint();
			break;
		case WM_SETTEXT:
			res = CallWindowProc(pControl->m_lpfnPrevWndProc, hWnd, Msg, wParam, lParam);
			pControl->m_bEmpty = (GetWindowTextLength(pControl->m_hWnd) == 0);
			pControl->Hint();
			return res;
		case WM_KEYDOWN:
			if (wParam == VK_RETURN)
				PostMessage(GetParent(pControl->m_hWnd), WM_COMMAND, MAKEWPARAM(0, EN_RETURN), (LPARAM)pControl->m_hWnd);
			break;
		}

		return CallWindowProc(pControl->m_lpfnPrevWndProc, hWnd, Msg, wParam, lParam);
	}

	void Hint()
	{
		// TODO: We should be able to change the edit control's text and prevent EN_CHANGE to be sent.

		if (!m_bEmpty)
			return;

		if (GetFocus() == m_hWnd)
		{
			CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETFONT, (WPARAM)(HFONT)m_fntNormal, FALSE);
			if (!GetText().IsEmpty())
				CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETTEXT, 0, NULL);
		}
		else
		{
			CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETFONT, (WPARAM)(HFONT)m_fntHint, FALSE);
			if (GetText() != m_strHint)
				CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETTEXT, 0, (LPARAM)(const TCHAR*)m_strHint);
		}
	}
	
	WNDPROC m_lpfnPrevWndProc;
	RFont m_fntNormal, m_fntHint;
	COLORREF m_clrNormal, m_clrHint, m_clrBackgr;
	RString m_strHint;
	bool m_bEmpty; // == m_bHintShownAtThisMoment
	HBRUSH m_hbrBackgr;
};
*/
