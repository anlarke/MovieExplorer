#pragma once

class CDatabasePage : public RPropertyPage
{
	friend class RWindow;

public:
	CDatabasePage();
	~CDatabasePage();
	virtual void ApplyChanges();

protected:
	bool OnCreate(CREATESTRUCT *pCS);
	void OnSize(DWORD type, WORD cx, WORD cy);
	void OnPrefChanged();
	void OnDrawItem(UINT_PTR id, DRAWITEMSTRUCT *pDIS);
	void OnMeasureItem(UINT_PTR id, MEASUREITEMSTRUCT *pMIS);
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);

	RStatic m_stcIndexExtensions, m_stcMaxInfoAge, m_stcOnlyUse, m_stcTitle, m_stcYear, 
			m_stcGenres, m_stcCountries, m_stcRuntime, m_stcStoryline, m_stcDirectors, 
			m_stcWriters, m_stcStars, m_stcPoster, m_stcRating;
	RComboBox m_cbOnlyUse, m_cbTitle, m_cbYear, m_cbGenres, m_cbCountries, m_cbRuntime, 
			m_cbStoryline, m_cbDirectors, m_cbWriters, m_cbStars, m_cbPoster, m_cbRating;
	REdit m_eIndexExtensions, m_eMaxInfoAge;
	RButton m_grpDatabase, m_grpInfoService, m_chkIndexDirectories;
	bool m_bInitialized;
};
