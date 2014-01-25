#pragma once

class CToolBarTreeView : public RWindow
{
	friend class RWindow;

public:
	CToolBarTreeView();
	~CToolBarTreeView();
	bool Create(HWND hWndParent, DWORD style, DWORD exStyle);
	void OnSize(DWORD type, WORD cx, WORD cy);
	void OnPaint(HDC hDC);

	RTreeView& GetTreeView() {return m_tv;}
	RCustomToolBar& GetToolBar() {return m_tb;}
	__declspec(property(get = GetTreeView)) RTreeView tv;
	__declspec(property(get = GetToolBar)) RCustomToolBar tb;

protected:
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);
	LRESULT OnNotify(UINT_PTR id, NMHDR *pNMH);

	void Draw();

	RTreeView m_tv;
	RCustomToolBar m_tb;
	RMemoryDC m_mdc;
};
