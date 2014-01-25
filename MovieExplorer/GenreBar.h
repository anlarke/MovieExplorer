#pragma once

#define BBS_NORMAL		0
#define BBS_HIGHLIGHT	1
#define BBS_SELECTED	2

////////////////////////////////////////////////////////////////////////////////////////////////////
// CButtonBar base class

class CButtonBar : public RWindow
{
	friend class RWindow;

public:
	CButtonBar()
	{
	}

	~CButtonBar()
	{
	}

protected:
	void OnPaint(HDC hDC)
	{
		m_mdc.Draw(hDC);
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		VERIFY(m_mdc.Create(cx, cy));
		Redraw();
	}

	INT_PTR AddButton(const RString& strText1, const RString& strText2)
	{
		BUTTON &but = *m_buttons.AddNew();
		but.strText1 = strText1;
		but.strText2 = strText2;
		but.state = BBS_NORMAL;
		Redraw();
		return m_buttons-1;
	}

	void Redraw(bool bFullRedraw = false)
	{
		if (bFullRedraw) // recreate sprites
		{







		}



		Invalidate(m_hWnd);
	}

	struct BUTTON
	{
		RString strText1, strText2;
		RRect rc;
		UINT_PTR state;
	};

	RMemoryDC m_mdc;
	RObArrayNRC<BUTTON> m_buttons;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// CGenreBar

class CGenreBar : public CButtonBar
{
	friend class RWindow;

public:
	CGenreBar()
	{
	}

	~CGenreBar()
	{
	}

protected:
	
};
