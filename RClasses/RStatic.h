#ifndef __RSTATIC_H__
#define __RSTATIC_H__

class RStatic : public RControl
{
	friend class RWindow;
	friend class RControl;

public:
	RStatic()
	{
	}

	~RStatic()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, DWORD style = 0, DWORD exStyle = 0, const TCHAR *lpszText = NULL, 
			int x = 0, int y = 0, int cx = 0, int cy = 0, UINT_PTR id = 0, void *pParam = NULL)
	{
		if (!RControl::Create<DERIVED_TYPE>(WC_STATIC, hWndParent, WS_CHILD|WS_VISIBLE|
				WS_CLIPCHILDREN|WS_CLIPSIBLINGS|style, exStyle, lpszText, x, y, cx, cy, id, pParam))
			ASSERTRETURN(false);

		SetFont(GetDialogFont());

		return true;
	}
};

inline void MoveStatic(const RStatic& st, int x, int y)
{
	RMemoryDC mdcTemp(0, 0);
	HFONT hPrevFont = (HFONT)SelectObject(mdcTemp, GetDialogFont());
	SIZE sz;
	GetTextExtentPoint32(mdcTemp, st.GetText(), &sz);
	SelectObject(mdcTemp, hPrevFont);
	MoveWindow(st, x, y, sz.cx + SCX(2), sz.cy + SCY(2));
}

#endif // __RSTATIC_H__
