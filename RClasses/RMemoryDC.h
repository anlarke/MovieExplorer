#ifndef __RMEMORYDC_H__
#define __RMEMORYDC_H__

class RMemoryDC
{
public:
	RMemoryDC() : m_cx(0), m_cy(0), m_hDC(NULL), m_hBmp(NULL), m_hOldBmp(NULL), m_pBits(NULL)
	{
	}

	RMemoryDC(int cx, int cy) : m_cx(0), m_cy(0), m_hDC(NULL), m_hBmp(NULL), m_hOldBmp(NULL), m_pBits(NULL)
	{
		Create(cx, cy);
	}

	RMemoryDC(const RMemoryDC& mdc) : m_cx(0), m_cy(0), m_hDC(NULL), m_hBmp(NULL), m_hOldBmp(NULL), m_pBits(NULL)
	{
		operator =(mdc);
	}

	~RMemoryDC()
	{
		Destroy();
	}

	bool Create(int cx, int cy)
	{
		if (cx < 0 || cy < 0)
		{
			Destroy();
			{ASSERT(false); return false;}
		}

		if (m_hDC && cx == m_cx && cy == m_cy)
		{
			if (cx > 0 && cy > 0)
				memset(m_pBits, 0, cx * cy * sizeof(RGBQUAD));

			SelectClipRgn(m_hDC, NULL); // remove any existing clipping on the dc
			return true;
		}

		if (!m_hDC)
		{
			m_hDC = CreateCompatibleDC(NULL);
			ASSERT(m_hDC);
			SetBkMode(m_hDC, TRANSPARENT);
		}

		SelectClipRgn(m_hDC, NULL); // remove any existing clipping on the dc

		if (m_hBmp)
		{
			SelectObject(m_hDC, m_hOldBmp);
			DeleteObject(m_hBmp);
			m_hBmp = m_hOldBmp = NULL;
			m_pBits = NULL;
		}

		if (cx > 0 && cy > 0)
		{
			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = cx;
			bmi.bmiHeader.biHeight = -cy; // top-down
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;

			m_hBmp = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, NULL);
			if (!m_hBmp)
			{
				Destroy();
				{ASSERT(false); return false;}
			}

			m_hOldBmp = (HBITMAP)SelectObject(m_hDC, m_hBmp);
		}

		m_cx = cx;
		m_cy = cy;
		
		return true;
	}

	void Destroy()
	{
		if (m_hDC)
		{
			if (m_hBmp)
			{
				VERIFY(SelectObject(m_hDC, m_hOldBmp));
				VERIFY(DeleteObject(m_hBmp));
			}
			VERIFY(DeleteDC(m_hDC));
		}
		m_hOldBmp = m_hBmp = NULL;
		m_pBits = NULL;
		m_hDC = NULL;
	}

	bool GetDimensions(int& cx, int& cy) const
	{
		if (!m_hDC)
			{ASSERT(false); return false;}

		cx = m_cx;
		cy = m_cy;

		return true;
	}

	bool GetDimensions(int *pnWidth, int *pnHeight) const
	{
		if (!m_hDC)
			{ASSERT(false); return false;}

		if (pnWidth)
			*pnWidth = m_cx;
		if (pnHeight)
			*pnHeight = m_cy;

		return true;
	}

	RGBQUAD* GetPixels() const // do not use inside a loop, call just before altering pixels and store pointer
	{
		ASSERT(m_hDC);
		GdiFlush();
		return m_pBits;
	}

	HBITMAP GetBitmap() const
	{
		ASSERT(m_hDC);
		return m_hBmp;
	}

	bool Clear()
	{
		ASSERT(m_hDC);
		RGBQUAD *pBits = GetPixels();
		if (pBits)
			memset(pBits, 0, m_cx * m_cy * sizeof(RGBQUAD));
		return true;
	}

	RMemoryDC& operator =(const RMemoryDC& mdc)
	{
		VERIFY(Create(mdc.m_cx, mdc.m_cy));
		GdiFlush();
		memcpy(m_pBits, mdc.m_pBits, m_cx * m_cy * sizeof(RGBQUAD));
		return *this;
	}
	
	operator HDC() const
	{
		return m_hDC;
	}

	bool Draw(HDC hDC, bool bUsingAlpha = false) const
	{
		if (bUsingAlpha)
		{
			static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			return AlphaBlend(hDC, 0, 0, m_cx, m_cy, m_hDC, 0, 0, m_cx, m_cy, bf);
		}
		return BitBlt(hDC, 0, 0, m_cx, m_cy, m_hDC, 0, 0, SRCCOPY);
	}

	bool Draw(HDC hDC, int x, int y, bool bUsingAlpha = false) const
	{
		if (bUsingAlpha)
		{
			static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			return AlphaBlend(hDC, x, y, m_cx, m_cy, m_hDC, 0, 0, m_cx, m_cy, bf);
		}
		return BitBlt(hDC, x, y, m_cx, m_cy, m_hDC, 0, 0, SRCCOPY);
	}

	const int& GetWidth() const
	{
		return m_cx;
	}

	const int& GetHeight() const
	{
		return m_cy;
	}

	__declspec(property(get = GetWidth)) int cx;
	__declspec(property(get = GetHeight)) int cy;

protected:
	int m_cx, m_cy;
	HDC m_hDC;
	HBITMAP m_hBmp, m_hOldBmp;
	RGBQUAD *m_pBits;
};

#endif // __RMEMORYDC_H__
