#ifndef __RSAVEDC_H__
#define __RSAVEDC_H__

class RSaveDC
{
public:
	RSaveDC(HDC hDC) : m_hDC(hDC), m_nSavedDC(SaveDC(hDC))
	{
	}

	~RSaveDC()
	{
		RestoreDC(m_hDC, m_nSavedDC);
	}

private:
	HDC m_hDC;
	int m_nSavedDC;
};

#endif // __RSAVEDC_H__
