#pragma once
#include "ToolBarButton.h"
#include "CategoryBar.h"
//#include "GenreBar.h"

class CReBar : public RWindow
{
	friend class RWindow;

public:
	CReBar();
	~CReBar();
	int GetHeight();

protected:
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);
	bool OnCreate(CREATESTRUCT *pCS);
	HBRUSH OnCtlColorEdit(HDC hDC, HWND hWnd);
	void OnDestroy();
	void OnPaint(HDC hDC);
	void OnPrefChanged();
	void OnScaleChanged();
	void OnSize(DWORD type, WORD cx, WORD cy);

	void Draw();
	static LRESULT CALLBACK LLMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	RMemoryDC m_mdc;
	bool m_bBackgrWinDefault, m_bInstantSearch, m_bLiteralSearch;
	COLORREF m_clrBackgrT, m_clrBackgrB, m_clrBackgrLine;
	CToolBarButton m_btnTools, m_btnSeen, m_btnSort, m_btnView;
	RMemoryDC m_mdcToolsBtn, m_mdcSeenBtn, m_mdcSortBtn, m_mdcViewBtn;
	HMENU m_hToolsMenu, m_hSortMenu;
	CCategoryBar m_categoryBar;
	RHintEdit m_eSearch;
	RFont m_fntSearch;
	RSprite m_sprBackgr, m_sprSearchBox;
	RString m_strSearchBoxFont;
	float m_fSearchBoxFontSize;
};
