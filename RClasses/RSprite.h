// RSprite.h : Defines the RSprite class.
//

#pragma once

class RSprite : public RMemoryDC
{
public:
	RSprite() : m_marginLeft(0), m_marginTop(0), m_marginRight(0), m_marginBottom(0)
	{
	}

	RSprite(int cx, int cy, int marginLeft = 0, int marginTop = 0, int marginRight = 0, int marginBottom = 0) : m_marginLeft(0), m_marginTop(0), m_marginRight(0), m_marginBottom(0)
	{
		Create(cx, cy, marginLeft, marginTop, marginRight, marginBottom);
	}

	~RSprite()
	{
	}

	bool Create(int cx, int cy, int marginLeft = 0, int marginTop = 0, int marginRight = 0, int marginBottom = 0)
	{
		if (marginLeft < 0 || marginTop < 0|| marginRight < 0 || marginBottom < 0 ||
				marginLeft > cx || marginTop > cy || marginRight > cx || marginBottom > cy ||
				marginLeft + marginRight > cx || marginTop + marginBottom > cy)
			{ASSERT(false); return false;}

		if (!RMemoryDC::Create(cx, cy))
			{ASSERT(false); return false;}
		
		m_marginLeft = marginLeft;
		m_marginTop = marginTop;
		m_marginRight = marginRight;
		m_marginBottom = marginBottom;

		return true;
	}

	bool Draw(HDC hDC, int x, int y, int cx, int cy, bool bUsingAlpha = false) const
	{
		if (bUsingAlpha)
		{
			static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			AlphaBlend(hDC, x + m_marginLeft, y + m_marginTop, cx - m_marginLeft - m_marginRight, cy - m_marginTop - m_marginBottom,
					   m_hDC, m_marginLeft, m_marginTop, m_cx - m_marginLeft - m_marginRight, m_cy - m_marginTop - m_marginBottom, bf);

			if (m_marginLeft)
			{
				if (m_marginTop)
					AlphaBlend(hDC, x, y, m_marginLeft, m_marginTop, m_hDC, 0, 0, m_marginLeft, m_marginTop, bf);
				if (m_marginBottom)
					AlphaBlend(hDC, x, y + cy - m_marginBottom, m_marginLeft, m_marginBottom, 
							   m_hDC, 0, m_cy - m_marginBottom, m_marginLeft, m_marginBottom, bf);
				
				AlphaBlend(hDC, x, y + m_marginTop, m_marginLeft, cy - m_marginTop - m_marginBottom, 
						   m_hDC, 0, m_marginTop, m_marginLeft, m_cy - m_marginTop - m_marginBottom, bf);
			}

			if (m_marginRight)
			{
				if (m_marginTop)
					AlphaBlend(hDC, x + cx - m_marginRight, y, m_marginRight, m_marginTop, 
							   m_hDC, m_cx - m_marginRight, 0, m_marginRight, m_marginTop, bf);
				if (m_marginBottom)
					AlphaBlend(hDC, x + cx - m_marginRight, y + cy - m_marginBottom, m_marginRight, m_marginBottom, 
							   m_hDC, m_cx - m_marginRight, m_cy - m_marginBottom, m_marginRight, m_marginBottom, bf);

				AlphaBlend(hDC, x + cx - m_marginRight, y + m_marginTop, m_marginRight, cy - m_marginTop - m_marginBottom,
						   m_hDC, m_cx - m_marginRight, m_marginTop, m_marginRight, m_cy - m_marginTop - m_marginBottom, bf);
			}

			if (m_marginTop)
				AlphaBlend(hDC, x + m_marginLeft, y, cx - m_marginLeft - m_marginRight, m_marginTop, 
						   m_hDC, m_marginLeft, 0, m_cx - m_marginLeft - m_marginRight, m_marginTop, bf);

			if (m_marginBottom)
				AlphaBlend(hDC, x + m_marginLeft, y + cy - m_marginBottom, cx - m_marginLeft - m_marginRight, m_marginBottom,
						   m_hDC, m_marginLeft, m_cy - m_marginBottom, m_cx - m_marginLeft - m_marginRight, m_marginBottom, bf);

		}
		else
		{
			StretchBlt(hDC, x + m_marginLeft, y + m_marginTop, cx - m_marginLeft - m_marginRight, cy - m_marginTop - m_marginBottom,
					   m_hDC, m_marginLeft, m_marginTop, m_cx - m_marginLeft - m_marginRight, m_cy - m_marginTop - m_marginBottom, SRCCOPY);

			if (m_marginLeft)
			{
				if (m_marginTop)
					BitBlt(hDC, x, y, m_marginLeft, m_marginTop, m_hDC, 0, 0, SRCCOPY);
				if (m_marginBottom)
					BitBlt(hDC, x, y + cy - m_marginBottom, m_marginLeft, m_marginBottom, m_hDC, 0, m_cy - m_marginBottom, SRCCOPY);
				
				StretchBlt(hDC, x, y + m_marginTop, m_marginLeft, cy - m_marginTop - m_marginBottom, 
						   m_hDC, 0, m_marginTop, m_marginLeft, m_cy - m_marginTop - m_marginBottom, SRCCOPY);
			}

			if (m_marginRight)
			{
				if (m_marginTop)
					BitBlt(hDC, x + cx - m_marginRight, y, m_marginRight, m_marginTop, m_hDC, m_cx - m_marginRight, 0, SRCCOPY);
				if (m_marginBottom)
					BitBlt(hDC, x + cx - m_marginRight, y + cy - m_marginBottom, m_marginRight, m_marginBottom, m_hDC, m_cx - m_marginRight, m_cy - m_marginBottom, SRCCOPY);

				StretchBlt(hDC, x + cx - m_marginRight, y + m_marginTop, m_marginRight, cy - m_marginTop - m_marginBottom,
						   m_hDC, m_cx - m_marginRight, m_marginTop, m_marginRight, m_cy - m_marginTop - m_marginBottom, SRCCOPY);
			}

			if (m_marginTop)
				StretchBlt(hDC, x + m_marginLeft, y, cx - m_marginLeft - m_marginRight, m_marginTop, 
						   m_hDC, m_marginLeft, 0, m_cx - m_marginLeft - m_marginRight, m_marginTop, SRCCOPY);

			if (m_marginBottom)
				StretchBlt(hDC, x + m_marginLeft, y + cy - m_marginBottom, cx - m_marginLeft - m_marginRight, m_marginBottom,
						   m_hDC, m_marginLeft, m_cy - m_marginBottom, m_cx - m_marginLeft - m_marginRight, m_marginBottom, SRCCOPY);
		}
		return true;
	}

	bool Draw(HDC hDC, RECT *pRect, bool bUsingAlpha = false) const
	{
		return Draw(hDC, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, bUsingAlpha);
	}

	bool Draw(HDC hDC, bool bUsingAlpha = false) const
	{
		return RMemoryDC::Draw(hDC, bUsingAlpha);
	}

	bool Draw(HDC hDC, int x, int y, bool bUsingAlpha = false) const
	{
		return RMemoryDC::Draw(hDC, x, y, bUsingAlpha);
	}

protected:
	int m_marginLeft, m_marginTop, m_marginRight, m_marginBottom;
};
