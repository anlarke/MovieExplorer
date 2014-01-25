#pragma once	

class CEditDlg : public RDialog
{
	friend class RWindow;
	friend class RDialog;

public:
	CEditDlg(HWND hWndParent, DBMOVIE *pMov);
	~CEditDlg();

protected:
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);
	bool OnCreate(CREATESTRUCT *pCS);
	void OnSize(DWORD type, WORD cx, WORD cy);
	void OnSizing(DWORD side, RECT *pRect);
	void OnOK();

	RButton m_btnOK, m_btnCancel, m_grpIdentification, m_grpInformation, m_grpOptionalProperties,
			m_chkSeenMovie, m_chkHideMovie;
	RStatic m_stcFileName, m_stcFileName2, m_stcFileSize, m_stcFileSize2, m_stcIMDb, 
			m_stcMovieMeter;
	REdit m_eIMDb, m_eMovieMeter;
	DBMOVIE *m_pMov;
};
