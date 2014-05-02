#pragma once

#define round(x)		(x < 0 ? (int)(x - 0.5) : (int)(x + 0.5))		// use for values up to about 15 digits
#define uround(x)		(x < 0 ? (UINT)(x - 0.5) : (UINT)(x + 0.5))
#define roundf(x)		(x < 0 ? (int)(x - 0.5f) : (int)(x + 0.5f))		// use for values up to about 7 digits
#define uroundf(x)		(x < 0 ? (UINT)(x - 0.5f) : (UINT)(x + 0.5f))

//template <typename T> T round(float val) {return val < 0.0f ? (T)(val - 0.5f) : (T)(val + 0.5f);}
//template <typename T> T round(double val) {return val < 0.0 ? (T)(val - 0.5) : (T)(val + 0.5);}

#define DRR_TL			1	// 0001
#define DRR_TR			2	// 0010
#define DRR_BL			4	// 0100
#define DRR_BR			8	// 1000

#define GS_AVERAGE		0
#define GS_LUMINANCE	1
#define GS_LUMINANCE2	2	// http://www.nbdtech.com/Blog/archive/2008/04/27/Calculating-the-Perceived-Brightness-of-a-Color.aspx

#define CLIPTOBYTE(x)	(BYTE)(x < 0 ? 0 : (x > 255 ? 255 : x))

inline bool AlphaBlt(HDC hDC, int x, int y, int cx, int cy, HDC hSrcDC, int xSrc = 0, int ySrc = 0)
{
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	return AlphaBlend(hDC, x, y, cx, cy, hSrcDC, xSrc, ySrc, cx, cy, bf) == TRUE;
}

inline bool AlphaBlt(HDC hDC, RECT *pRect, HDC hSrcDC, int xSrc = 0, int ySrc = 0)
{
	static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	int cx = pRect->right - pRect->left;
	int cy = pRect->bottom - pRect->top;
	return AlphaBlend(hDC, pRect->left, pRect->top, cx, cy, hSrcDC, xSrc, ySrc, cx, cy, bf) == TRUE;
}

inline bool AlphaBlt(HDC hDC, int x, int y, int cx, int cy, HDC hSrcDC, 
		int xSrc, int ySrc, int cxSrc, int cySrc, BYTE alpha = 255)
{
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
	return AlphaBlend(hDC, x, y, cx, cy, hSrcDC, xSrc, ySrc, cxSrc, cySrc, bf) == TRUE;
}

inline COLORREF BlendColor(COLORREF clrBase, COLORREF clrMix, float fMix)
{
	return RGB(roundf(GetRValue(clrBase) * (1.0f - fMix) + GetRValue(clrMix) * fMix),
			   roundf(GetGValue(clrBase) * (1.0f - fMix) + GetGValue(clrMix) * fMix),
			   roundf(GetBValue(clrBase) * (1.0f - fMix) + GetBValue(clrMix) * fMix));
}

inline bool DrawRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clr, BYTE alpha = 255, bool bSolid = true)
{
	if (!hDC)
		{ASSERT(false); return false;}
	if (cx == 0 || cy == 0 || alpha == 0)
		return true;
	if (cx < 0 || cy < 0)
		return false;

	static RMemoryDC mdc(1, 1);
	static RGBQUAD *pPixel = mdc.GetPixels();
	pPixel->rgbRed = (GetRValue(clr) * alpha) / 255;
	pPixel->rgbGreen = (GetGValue(clr) * alpha) / 255;
	pPixel->rgbBlue = (GetBValue(clr) * alpha) / 255;
	pPixel->rgbReserved = alpha;
	
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, alpha == 255 ? 0 : AC_SRC_ALPHA};
	return bSolid ? AlphaBlend(hDC, x, y, cx, cy, mdc, 0, 0, 1, 1, bf) :
					(AlphaBlend(hDC, x, y, cx, SCY(1), mdc, 0, 0, 1, 1, bf) &&
					 AlphaBlend(hDC, x, y + SCY(1), SCX(1), cy - SCY(2), mdc, 0, 0, 1, 1, bf) &&
					 AlphaBlend(hDC, x + cx - SCX(1), y + SCY(1), SCX(1), cy - SCY(2), mdc, 0, 0, 1, 1, bf) &&
					 AlphaBlend(hDC, x, y + cy - SCY(1), cx, SCY(1), mdc, 0, 0, 1, 1, bf));
}

inline bool DrawRect(HDC hDC, RECT *pRect, COLORREF clr, BYTE alpha = 255, bool bSolid = true)
	{return DrawRect(hDC, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, clr, alpha, bSolid);}

inline bool DrawRectGradientV(HDC hDC, int x, int y, int cx, int cy, COLORREF clrTop, COLORREF clrBottom, 
							  BYTE alphaTop = 255, BYTE alphaBottom = 255, bool bSolid = true)
{
	if (clrTop == clrBottom && alphaTop == alphaBottom)
		return DrawRect(hDC, x, y, cx, cy, clrTop, alphaTop, bSolid);

	if (!hDC)
		{ASSERT(false); return false;}
	if (cx == 0 || cy == 0 || (alphaTop == 0 && alphaBottom == 0))
		return true;
	if (cx < 0 || cy < 0)
		{ASSERT(false); return false;}

	RMemoryDC mdc(1, cy);
	RGBQUAD *pPixels = mdc.GetPixels();
	
	int i;
	BYTE alpha;
	COLORREF clr;
	RGBQUAD *pPixel;

	for (i = 0; i < cy; i++)
	{
		if (alphaTop == alphaBottom)
			alpha = alphaTop;
		else
			alpha = (BYTE)((i * alphaBottom + ((cy-1) - i) * alphaTop) / (cy-1));

		if (clrTop == clrBottom)
			clr = clrTop;
		else
			clr = BlendColor(clrTop, clrBottom, (float)i / (cy-1));

		pPixel = pPixels + i;
		pPixel->rgbRed = (GetRValue(clr) * alpha) / 255;
		pPixel->rgbGreen = (GetGValue(clr) * alpha) / 255;
		pPixel->rgbBlue = (GetBValue(clr) * alpha) / 255;
		pPixel->rgbReserved = alpha;
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, (alphaTop == 255 && alphaBottom == 255) ? 0 : AC_SRC_ALPHA};
	return bSolid ? AlphaBlend(hDC, x, y, cx, cy, mdc, 0, 0, 1, cy, bf) != FALSE :
					(AlphaBlend(hDC, x, y, cx, SCY(1), mdc, 0, 0, 1, 1, bf) &&
					 AlphaBlend(hDC, x, y + SCY(1), SCX(1), cy - SCY(2), mdc, 0, 1, 1, cy - 2, bf) &&
					 AlphaBlend(hDC, x + cx - SCX(1), y + SCY(1), SCX(1), cy - SCY(2), mdc, 0, 0, 1, cy - 2, bf) &&
					 AlphaBlend(hDC, x, y + cy - SCY(1), cx, SCY(1), mdc, 0, cy - 1, 1, 1, bf));
}

inline bool DrawRectGradientH(HDC hDC, int x, int y, int cx, int cy, COLORREF clrLeft, COLORREF clrRight, 
							  BYTE alphaLeft = 255, BYTE alphaRight = 255, bool bSolid = true)
{
	if (clrLeft == clrRight && alphaLeft == alphaRight)
		return DrawRect(hDC, x, y, cx, cy, clrLeft, alphaLeft, bSolid);

	if (!hDC)
		{ASSERT(false); return false;}
	if (cx == 0 || cy == 0 || (alphaLeft == 0 && alphaRight == 0))
		return true;
	if (cx < 0 || cy < 0)
		{ASSERT(false); return false;}

	RMemoryDC mdc(cx, 1);
	RGBQUAD *pPixels = mdc.GetPixels();
	
	int i;
	BYTE alpha;
	COLORREF clr;
	RGBQUAD *pPixel;

	for (i = 0; i < cx; i++)
	{
		if (alphaLeft == alphaRight)
			alpha = alphaLeft;
		else
			alpha = (BYTE)((cx-1) == 0 ? 0 : (i * alphaRight + ((cx-1) - i) * alphaLeft) / (cx-1));

		if (clrLeft == clrRight)
			clr = clrLeft;
		else
			clr = BlendColor(clrLeft, clrRight, (float)i / (cx-1));

		pPixel = pPixels + i;
		pPixel->rgbRed = (GetRValue(clr) * alpha) / 255;
		pPixel->rgbGreen = (GetGValue(clr) * alpha) / 255;
		pPixel->rgbBlue = (GetBValue(clr) * alpha) / 255;
		pPixel->rgbReserved = alpha;
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, (alphaLeft == 255 && alphaRight == 255) ? 0 : AC_SRC_ALPHA};
	return bSolid ? AlphaBlend(hDC, x, y, cx, cy, mdc, 0, 0, cx, 1, bf) != FALSE :
					false;
					// dit moet ik nog eens omschrijven...
					//(AlphaBlend(hDC, x, y, cx, 1, mdc, 0, 0, 1, 1, bf) &&
					// AlphaBlend(hDC, x, y, 1, cy, mdc, 0, 0, 1, cy, bf) &&
					// AlphaBlend(hDC, x + cx - 1, y, 1, cy, mdc, 0, 0, 1, cy, bf) &&
					// AlphaBlend(hDC, x, y + cy - 1, cx, 1, mdc, 0, cy - 1, 1, 1, bf));
}

inline bool DrawCircle(HDC hDC, int x, int y, int r, COLORREF clr, BYTE alpha = 255, bool bSolid = true)
{
	if (!hDC)
		{ASSERT(false); return false;}
	if (r <= 0)
		{ASSERT(false); return false;}
	if (alpha == 0)
		return true;

	RMemoryDC mdc(r, r);
	RGBQUAD *pPixels = mdc.GetPixels();
	memset(pPixels, 0, r * r * sizeof(RGBQUAD));
	
	RGBQUAD *pPixel;
	int i, j, ymin, ymax;
	double a, b;

	for (i = 0; i < r; i++)  // i = x
	{
		ymax = (i == 0) ? r - 1 : (int)sqrt(pow(r, 2.0) - pow(i, 2.0));
		ymin = (int)sqrt(pow(r, 2.0) - pow(i + 1, 2.0));

		for (j = 0; j <= ymax; j++)  // j = y
		{
			pPixel = pPixels + j * r + i;
			if (j < ymin)
				pPixel->rgbReserved = alpha;
			else
			{
				a = sqrt(pow(r, 2.0) - pow(j + 1, 2.0));
				if (a < i)
					a = i;
				b = sqrt(pow(r, 2.0) - pow(j, 2.0));
				if (b > i + 1)
					b = i + 1;

				pPixel->rgbReserved = (BYTE)uroundf(alpha * (0.5 * (r*r * asin(b/r) - r*r * asin(a/r) 
					- a * sqrt(r*r - a*a) + b * sqrt(r*r - b*b)) + j * (a - b) + a - i));
			}
			pPixel->rgbRed = (GetRValue(clr) * pPixel->rgbReserved) / 0xFF;
			pPixel->rgbGreen = (GetGValue(clr) * pPixel->rgbReserved) / 0xFF;
			pPixel->rgbBlue = (GetBValue(clr) * pPixel->rgbReserved) / 0xFF;
		}
	}

	if (!bSolid && r > 1)
	{
		RMemoryDC mdcInner(r, r);
		RGBQUAD *pInnerPixels = mdcInner.GetPixels();
		memset(pInnerPixels, 0, r * r * sizeof(RGBQUAD));

		r--;
		for (i = 0; i < r; i++)  // i = x
		{
			ymax = (i == 0) ? r - 1 : (int)sqrt(pow(r, 2.0) - pow(i, 2.0));
			ymin = (int)sqrt(pow(r, 2.0) - pow(i + 1, 2.0));

			for (j = 0; j <= ymax; j++)  // j = y
			{
				pPixel = pInnerPixels + j * (r + 1) + i; // outline correctly, we changed r!
				if (j < ymin)
					pPixel->rgbReserved = alpha;
				else
				{
					a = sqrt(pow(r, 2.0) - pow(j + 1, 2.0));
					if (a < i)
						a = i;
					b = sqrt(pow(r, 2.0) - pow(j, 2.0));
					if (b > i + 1)
						b = i + 1;

					pPixel->rgbReserved = (BYTE)uroundf(alpha * (0.5 * (r*r * asin(b/r) - r*r * asin(a/r) 
						- a * sqrt(r*r - a*a) + b * sqrt(r*r - b*b)) + j * (a - b) + a - i));
				}
			}
		}
		r++;

		for (i = 0; i < r * r; i++)
		{
			pPixel = pPixels + i;

			pPixel->rgbReserved -= (pInnerPixels + i)->rgbReserved;
			pPixel->rgbRed = (GetRValue(clr) * pPixel->rgbReserved) / 0xFF;
			pPixel->rgbGreen = (GetGValue(clr) * pPixel->rgbReserved) / 0xFF;
			pPixel->rgbBlue = (GetBValue(clr) * pPixel->rgbReserved) / 0xFF;
		}
	}

	int cx, cy;
	cx = cy = r * 2;
	RMemoryDC mdcCircle(cx, cy);
	RGBQUAD *pCirclePixels = mdcCircle.GetPixels();

	for (i = 0; i < r; i++)
	{
		for (j = 0; j < r; j++)
		{
			pPixel = pPixels + j * r + i;
			*(pCirclePixels + (r + j) * 2 * r + r + i) = *pPixel;
			*(pCirclePixels + (r + j) * 2 * r + r - i - 1) = *pPixel;
		}
	}
	for (j = 0; j < r; j++)
		memcpy(pCirclePixels + j * cx, pCirclePixels + (cy - j - 1) * cx, cx * sizeof(RGBQUAD));

	static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	return AlphaBlend(hDC, x, y, cx, cy, mdcCircle, 0, 0, cx, cy, bf) != FALSE;
}

inline bool DrawRoundedRect(HDC hDC, int x, int y, int cx, int cy, int r, COLORREF clr, BYTE alpha = 255, 
							bool bSolid = true, DWORD dwroundf = DRR_TL|DRR_TR|DRR_BL|DRR_BR)
{
	if (!hDC || cx < 0 || cy < 0 || r < 0)
		{ASSERT(false); return false;}
	if (cx == 0 || cy == 0 || alpha == 0)
		return true;
	if (r == 0 || dwroundf == 0)
		return DrawRect(hDC, x, y, cx, cy, clr, alpha, bSolid);


	//if (cx < r * 2 || cy < r * 2)
	//	{ASSERT(false); return false;} // still need to find a proper solution for this!


	RMemoryDC mdcCircle(r * 2, r * 2);
	RGBQUAD *pPixelsCircle = mdcCircle.GetPixels();
	memset(pPixelsCircle, 0, r * 2 * r * 2 * sizeof(RGBQUAD));

	if (!DrawCircle(mdcCircle, 0, 0, r, clr, alpha, bSolid))
		{ASSERT(false); return false;}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	if (dwroundf & DRR_TL) AlphaBlend(hDC, x, y, r, r, mdcCircle, 0, 0, r, r, bf);
	if (dwroundf & DRR_TR) AlphaBlend(hDC, x + cx - r, y, r, r, mdcCircle, r, 0, r, r, bf);
	if (dwroundf & DRR_BL) AlphaBlend(hDC, x, y + cy - r, r, r, mdcCircle, 0, r, r, r, bf);
	if (dwroundf & DRR_BR) AlphaBlend(hDC, x + cx - r, y + cy - r, r, r, mdcCircle, r, r, r, r, bf);

	if (bSolid)
	{
		DrawRect(hDC, x + (dwroundf & DRR_TL ? r : 0), y, cx - (dwroundf & DRR_TL ? r : 0) - (dwroundf & DRR_TR ? r : 0), r, clr, alpha);
		DrawRect(hDC, x, y + r, cx, cy - r * 2, clr, alpha);
		DrawRect(hDC, x + (dwroundf & DRR_BL ? r : 0), y + cy - r, cx - (dwroundf & DRR_BL ? r : 0) - (dwroundf & DRR_BR ? r : 0), r, clr, alpha);
	}
	else
	{
		// TODO: Use SCX(1) instead of 1
		DrawRect(hDC, x + (dwroundf & DRR_TL ? r : 0), y, cx - (dwroundf & DRR_TL ? r : 0) - (dwroundf & DRR_TR ? r : 0), 1, clr, alpha);
		DrawRect(hDC, x, y + (dwroundf & DRR_TL ? r : 1), 1, cy - (dwroundf & DRR_TL ? r : 1) - (dwroundf & DRR_BL ? r : 1), clr, alpha);
		DrawRect(hDC, x + (dwroundf & DRR_BL ? r : 0), y + cy - 1, cx - (dwroundf & DRR_BL ? r : 0) - (dwroundf & DRR_BR ? r : 0), 1, clr, alpha);
		DrawRect(hDC, x + cx - 1, y + (dwroundf & DRR_TR ? r : 1), 1, cy - (dwroundf & DRR_TR ? r : 1) - (dwroundf & DRR_BR ? r : 1), clr, alpha);
	}
	
	return true;
}

inline bool DrawRoundedRectGradientV(HDC hDC, int x, int y, int cx, int cy, int r, COLORREF clrTop, COLORREF clrBottom, BYTE alphaTop = 255, 
									 BYTE alphaBottom = 255, bool bSolid = true, DWORD dwroundf = DRR_TL|DRR_TR|DRR_BL|DRR_BR)
{
	if (!hDC || cx < 0 || cy < 0 || r < 0)
		{ASSERT(false); return false;}
	if (cx == 0 || cy == 0 || (alphaTop == 0 && alphaBottom == 0))
		return true;
	if (r == 0 || dwroundf == 0)
		return DrawRectGradientV(hDC, x, y, cx, cy, clrTop, clrBottom, alphaTop, alphaBottom, bSolid);
	if (clrTop == clrBottom && alphaTop == alphaBottom)
		return DrawRoundedRect(hDC, x, y, cx, cy, r, clrTop, alphaTop, bSolid, dwroundf);

	if (cx < r * 2 || cy < r * 2)
		{ASSERT(false); return false;} // still need to find a proper solution for this!

	int cxTemp = r * 2 + 1;
	RMemoryDC mdcTemp(cxTemp, cy);
	RGBQUAD *pPixels = mdcTemp.GetPixels();

	DrawRoundedRect(mdcTemp, 0, 0, cxTemp, cy, r, 0, 255, bSolid, dwroundf);

	int i, j;
	RGBQUAD *pPixel;
	BYTE alpha;
	COLORREF clr;

	for (j = 0; j < cy; j++)
	{
		if (alphaTop == alphaBottom)
			alpha = alphaTop;
		else
			alpha = (BYTE)((j * alphaBottom + ((cy-1) - j) * alphaTop) / (cy-1));

		if (clrTop == clrBottom)
			clr = clrTop;
		else
			clr = BlendColor(clrTop, clrBottom, (float)j / (cy-1));

		for (i = 0; i < cxTemp; i++)
		{
			pPixel = pPixels + j * cxTemp + i;
			pPixel->rgbReserved = (pPixel->rgbReserved * alpha) / 255;
			pPixel->rgbRed = (GetRValue(clr) * pPixel->rgbReserved) / 255;
			pPixel->rgbGreen = (GetGValue(clr) * pPixel->rgbReserved) / 255;
			pPixel->rgbBlue = (GetBValue(clr) * pPixel->rgbReserved) / 255;
		}
	}

	static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	return (AlphaBlend(hDC, x, y, r, cy, mdcTemp, 0, 0, r, cy, bf) &&
			AlphaBlend(hDC, x + r, y, cx - 2 * r, cy, mdcTemp, r, 0, 1, cy, bf) &&
			AlphaBlend(hDC, x + cx - r, y, r, cy, mdcTemp, r + 1, 0, r, cy, bf));
}

inline bool DrawRoundedRectGradientH(HDC hDC, int x, int y, int cx, int cy, int r, 
		COLORREF clrLeft, COLORREF clrRight, BYTE alphaLeft = 255, BYTE alphaRight = 255, 
		bool bSolid = true, DWORD dwroundf = DRR_TL|DRR_TR|DRR_BL|DRR_BR)
{
	if (!hDC || cx < 0 || cy < 0 || r < 0)
		{ASSERT(false); return false;}
	if (cx == 0 || cy == 0 || (alphaLeft == 0 && alphaRight == 0))
		return true;
	if (r == 0 || dwroundf == 0)
		return DrawRectGradientH(hDC, x, y, cx, cy, clrLeft, clrRight, alphaLeft, alphaRight, bSolid);
	if (clrLeft == clrRight && alphaLeft == alphaRight)
		return DrawRoundedRect(hDC, x, y, cx, cy, r, clrLeft, alphaLeft, bSolid, dwroundf);

	if (cx < r * 2 || cy < r * 2)
		{ASSERT(false); return false;} // still need to find a proper solution for this!

	int cyTemp = r * 2 + 1;
	RMemoryDC mdcTemp(cx, cyTemp);
	RGBQUAD *pPixels = mdcTemp.GetPixels();

	DrawRoundedRect(mdcTemp, 0, 0, cx, cyTemp, r, 0, 255, bSolid, dwroundf);

	int i, j;
	RGBQUAD *pPixel;
	BYTE alpha;
	COLORREF clr;

	for (i = 0; i < cx; ++i)
	{
		if (alphaLeft == alphaRight)
			alpha = alphaLeft;
		else
			alpha = (BYTE)((i * alphaRight + ((cx-1) - i) * alphaLeft) / (cx-1));

		if (clrLeft == clrRight)
			clr = clrLeft;
		else
			clr = BlendColor(clrLeft, clrRight, (float)i / (cx-1));

		for (j = 0; j < cyTemp; ++j)
		{
			pPixel = pPixels + j * cx + i;
			pPixel->rgbReserved = (pPixel->rgbReserved * alpha) / 255;
			pPixel->rgbRed = (GetRValue(clr) * pPixel->rgbReserved) / 255;
			pPixel->rgbGreen = (GetGValue(clr) * pPixel->rgbReserved) / 255;
			pPixel->rgbBlue = (GetBValue(clr) * pPixel->rgbReserved) / 255;
		}
	}

	static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	return (AlphaBlend(hDC, x, y, cx, r, mdcTemp, 0, 0, cx, r, bf) &&
			AlphaBlend(hDC, x, y + r, cx, cy - 2 * r, mdcTemp, 0, r, cx, 1, bf) &&
			AlphaBlend(hDC, x, y + cy - r, cx, r, mdcTemp, 0, r + 1, cx, r, bf));
}

inline bool DrawAlphaMap(HDC hDC, int x, int y, const BYTE *pMap, int cxMap, int cyMap, COLORREF clr, BYTE alpha = 255)
{
	RMemoryDC mdc;
	if (!mdc.Create(cxMap, cyMap))
		{ASSERT(false); return false;}

	RGBQUAD *pPixels = mdc.GetPixels();
	RGBQUAD *pPixel;
	for (int i = 0; i < cxMap * cyMap; i++)
	{
		pPixel = pPixels + i;
		pPixel->rgbReserved = (*(pMap + i) * alpha) / 255;
		pPixel->rgbRed = (GetRValue(clr) * pPixel->rgbReserved) / 255;
		pPixel->rgbGreen = (GetGValue(clr) * pPixel->rgbReserved) / 255;
		pPixel->rgbBlue = (GetBValue(clr) * pPixel->rgbReserved) / 255;
	}

	static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	return AlphaBlend(hDC, x, y, cxMap, cyMap, mdc, 0, 0, cxMap, cyMap, bf) != FALSE;
}

inline bool DrawAlphaMap(HDC hDC, int x, int y, WORD mapResID, int cxMap, int cyMap, COLORREF clr, BYTE alpha = 255)
{
	HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(mapResID), _T("AlphaMap"));
	if (!hResInfo)
		{ASSERT(false); return false;}
	HGLOBAL hResData = LoadResource(NULL, hResInfo);
	if (!hResData)
		{ASSERT(false); return false;}
	bool bResult = DrawAlphaMap(hDC, x, y, (BYTE*)LockResource(hResData), cxMap, cyMap, clr, alpha);
	UnlockResource(hResData);
	return bResult;
}

inline void Grayscale(RMemoryDC *pMemDC, DWORD dwMethod = GS_LUMINANCE)
{
	int cx, cy;
	pMemDC->GetDimensions(cx, cy);
	int nPixels = cx * cy;
	RGBQUAD *pPixels = pMemDC->GetPixels();
	RGBQUAD *pPixel;
	
	if (dwMethod == GS_AVERAGE)
	{
		for (int i = 0; i < nPixels; ++i)
		{
			pPixel = pPixels + i;
			pPixel->rgbRed = pPixel->rgbGreen = pPixel->rgbBlue = (BYTE)(((int)pPixel->rgbRed + 
					(int)pPixel->rgbGreen + (int)pPixel->rgbBlue) / 3);
		}
	}
	else if (dwMethod == GS_LUMINANCE)
	{
		for (int i = 0; i < nPixels; ++i)
		{
			pPixel = pPixels + i;
			pPixel->rgbRed = pPixel->rgbGreen = pPixel->rgbBlue = (BYTE)(((int)pPixel->rgbRed * 299 + 
					(int)pPixel->rgbGreen * 587 + (int)pPixel->rgbBlue * 114) / 1000);
		}
	}
	else if (dwMethod == GS_LUMINANCE2)
	{
		for (int i = 0; i < nPixels; ++i)
		{
			pPixel = pPixels + i;
			pPixel->rgbRed = pPixel->rgbGreen = pPixel->rgbBlue = (BYTE)sqrt(
						(float)pPixel->rgbRed * (float)pPixel->rgbRed * 0.241f + 
						(float)pPixel->rgbGreen * (float)pPixel->rgbGreen * 0.691f + 
						(float)pPixel->rgbBlue * (float)pPixel->rgbBlue * 0.068f
					);
		}
	}
	else
		ASSERT(false);
}

inline void Flip(RMemoryDC *pMemDC)
{
	const size_t cx = pMemDC->GetWidth();
	const size_t cy = pMemDC->GetHeight();
	RGBQUAD *pPixels = pMemDC->GetPixels();
	RGBQUAD temp;
	RGBQUAD *pt, *pt_max, *pb;
	for (size_t y = 0; y < cy / 2; ++y)
	{
		for (pt = pPixels + y * cx, pt_max = pt + cx, 
				pb = pPixels + (cy - 1 - y) * cx; pt < pt_max; ++pt, ++pb)
		{
			temp = *pt;
			*pt = *pb;
			*pb = temp;
		}
	}
}

inline RMemoryDC Flip(const RMemoryDC& memDC)
	{RMemoryDC res(memDC); Flip(&res); return res;}

inline void Mirror(RMemoryDC *pMemDC)
{
	const size_t cx = pMemDC->GetWidth();
	const size_t cy = pMemDC->GetHeight();
	RGBQUAD *pPixels = pMemDC->GetPixels();
	RGBQUAD temp;
	size_t x, y;
	for (y = 0; y < cy; ++y)
	{
		for (x = 0; x < cx / 2; ++x)
		{
			temp = pPixels[y * cx + x];
			pPixels[y * cx + x] = pPixels[y * cx + cx - 1 - x];
			pPixels[y * cx + cx - 1 - x] = temp;
		}
	}
}

inline RMemoryDC Mirror(const RMemoryDC& memDC)
	{RMemoryDC res(memDC); Mirror(&res); return res;}

inline void Brightness(RMemoryDC *pMemDC, int nCorrection, bool bAlpha = false)
{
	if (nCorrection == 0)
		return;

	ASSERT(nCorrection >= -255 && nCorrection <= 255);
	nCorrection = (nCorrection < -255 ? -255 : (nCorrection > 255 ? 255 : nCorrection));
	
	int cx, cy;
	pMemDC->GetDimensions(cx, cy);
	int nPixels = cx * cy;
	RGBQUAD *pPixels = pMemDC->GetPixels();
	RGBQUAD *pPixel;
	int c;

	if (bAlpha)
	{
		for (int i = 0; i < nPixels; ++i)
		{
			pPixel = pPixels + i;

			if (pPixel->rgbReserved == 0)
				continue;

			c = (255 * pPixel->rgbRed) / pPixel->rgbReserved + nCorrection;
			pPixel->rgbRed = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;

			c = (255 * pPixel->rgbGreen) / pPixel->rgbReserved + nCorrection;
			pPixel->rgbGreen = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;

			c = (255 * pPixel->rgbBlue) / pPixel->rgbReserved + nCorrection;
			pPixel->rgbBlue = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;
		}
	}
	else
	{
		for (int i = 0; i < nPixels; ++i)
		{
			pPixel = pPixels + i;

			c = pPixel->rgbRed + nCorrection;
			pPixel->rgbRed = CLIPTOBYTE(c);

			c = pPixel->rgbGreen + nCorrection;
			pPixel->rgbGreen = CLIPTOBYTE(c);

			c = pPixel->rgbBlue + nCorrection;
			pPixel->rgbBlue = CLIPTOBYTE(c);
		}
	}
}

inline void Contrast(RMemoryDC *pMemDC, int nCorrection, bool bAlpha = false)
{
	if (nCorrection == 0)
		return;
	
	ASSERT(nCorrection >= -100 && nCorrection <= 100);
	nCorrection = (nCorrection < -100 ? -100 : (nCorrection > 100 ? 100 : nCorrection));
	
	int cx, cy;
	pMemDC->GetDimensions(cx, cy);
	int nPixels = cx * cy;
	RGBQUAD *pPixels = pMemDC->GetPixels();
	RGBQUAD *pPixel;
	int c;

	if (nCorrection > 0)
	{
		if (bAlpha)
		{
			for (int i = 0; i < nPixels; ++i)
			{
				pPixel = pPixels + i;

				if (pPixel->rgbReserved == 0)
					continue;

				c = (255 * pPixel->rgbRed) / pPixel->rgbReserved;
				c = (25500 * (int)c - 32385 * nCorrection) / (25500 - 254 * nCorrection);
				pPixel->rgbRed = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;

				c = (255 * pPixel->rgbGreen) / pPixel->rgbReserved;
				c = (25500 * (int)c - 32385 * nCorrection) / (25500 - 254 * nCorrection);
				pPixel->rgbGreen = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;

				c = (255 * pPixel->rgbBlue) / pPixel->rgbReserved;
				c = (25500 * (int)c - 32385 * nCorrection) / (25500 - 254 * nCorrection);
				pPixel->rgbBlue = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;
			}
		}
		else
		{
			for (int i = 0; i < nPixels; ++i)
			{
				pPixel = pPixels + i;

				c = (25500 * (int)pPixel->rgbRed - 32385 * nCorrection) / (25500 - 254 * nCorrection);
				pPixel->rgbRed = CLIPTOBYTE(c);

				c = (25500 * (int)pPixel->rgbGreen - 32385 * nCorrection) / (25500 - 254 * nCorrection);
				pPixel->rgbGreen = CLIPTOBYTE(c);

				c = (25500 * (int)pPixel->rgbBlue - 32385 * nCorrection) / (25500 - 254 * nCorrection);
				pPixel->rgbBlue = CLIPTOBYTE(c);
			}
		}
	}
	else
	{
		if (bAlpha)
		{
			for (int i = 0; i < nPixels; ++i)
			{
				pPixel = pPixels + i;

				if (pPixel->rgbReserved == 0)
					continue;

				c = (255 * pPixel->rgbRed) / pPixel->rgbReserved;
				c = (int)c + ((254 * (int)c - 32385) * nCorrection) / 25500;
				pPixel->rgbRed = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;

				c = (255 * pPixel->rgbGreen) / pPixel->rgbReserved;
				c = (int)c + ((254 * (int)c - 32385) * nCorrection) / 25500;
				pPixel->rgbGreen = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;

				c = (255 * pPixel->rgbBlue) / pPixel->rgbReserved;
				c = (int)c + ((254 * (int)c - 32385) * nCorrection) / 25500;
				pPixel->rgbBlue = (CLIPTOBYTE(c) * pPixel->rgbReserved) / 255;
			}
		}
		else
		{
			for (int i = 0; i < nPixels; ++i)
			{
				pPixel = pPixels + i;

				c = (int)pPixel->rgbRed + ((254 * (int)pPixel->rgbRed - 32385) * nCorrection) / 25500;
				pPixel->rgbRed = CLIPTOBYTE(c);

				c = (int)pPixel->rgbGreen + ((254 * (int)pPixel->rgbGreen - 32385) * nCorrection) / 25500;
				pPixel->rgbGreen = CLIPTOBYTE(c);

				c = (int)pPixel->rgbBlue + ((254 * (int)pPixel->rgbBlue - 32385) * nCorrection) / 25500;
				pPixel->rgbBlue = CLIPTOBYTE(c);
			}
		}
	}
}

#if defined(__wincodec_h__) && defined(__RMEMORYDC_H__) && defined(__RCOMPTR_H__)
inline bool Resize(RMemoryDC *pDestDC, RMemoryDC *pSrcDC, bool bAlpha = false)
{
	ASSERT(pDestDC);
	ASSERT((HDC)*pDestDC);
	ASSERT(pSrcDC);
	ASSERT((HDC)*pSrcDC);

	static BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	int cxDest, cyDest, cxSrc, cySrc;
	VERIFY(pDestDC->GetDimensions(cxDest, cyDest));
	VERIFY(pSrcDC->GetDimensions(cxSrc, cySrc));

	if (cxDest == 0 || cyDest == 0 || cxSrc == 0 || cySrc == 0)
		return true;

	// Just copy when size doesn't change

	if (cxDest == cxSrc && cyDest == cySrc)
	{
		if (bAlpha)
			return BitBlt(*pDestDC, 0, 0, cxDest, cyDest, *pSrcDC, 0, 0, SRCCOPY);
		else
			return AlphaBlend(*pDestDC, 0, 0, cxDest, cyDest, *pSrcDC, 0, 0, cxSrc, cySrc, bf);
	}

	// Try to use the Windows Imaging Component

	RComPtr<IWICImagingFactory> pImgFac;
	if (SUCCEEDED(pImgFac.CoCreateInstance(CLSID_WICImagingFactory)))
	{
		RComPtr<IWICBitmap> pBitmap;
		if (FAILED(pImgFac->CreateBitmapFromHBITMAP(pSrcDC->GetBitmap(), NULL,
				(bAlpha ? WICBitmapUsePremultipliedAlpha : WICBitmapIgnoreAlpha), &pBitmap)))
			{ASSERT(false); return false;}

		RComPtr<IWICBitmapScaler> pScaler;
		if (FAILED(pImgFac->CreateBitmapScaler(&pScaler)))
			 {ASSERT(false); return false;}

		if (FAILED(pScaler->Initialize(pBitmap, cxDest, cyDest, WICBitmapInterpolationModeFant)))
			{ASSERT(false); return false;}

		WICRect rc = {0, 0, cxDest, cyDest};
		if (FAILED(pScaler->CopyPixels(&rc, cxDest * 4, cxDest * cyDest * 4, (BYTE*)pDestDC->GetPixels())))
			{ASSERT(false); return false;}

		return true;
	}
	
	// Otherwise just use the GDI (lower quality and slower)

	if (bAlpha)
		return AlphaBlend(*pDestDC, 0, 0, cxDest, cyDest, *pSrcDC, 0, 0, cxSrc, cySrc, bf);
	else
	{
		int nPrevMode = SetStretchBltMode(*pDestDC, HALFTONE);
		StretchBlt(*pDestDC, 0, 0, cxDest, cyDest, *pSrcDC, 0, 0, cxSrc, cySrc, SRCCOPY);
		SetStretchBltMode(*pDestDC, nPrevMode);
	}

	return true;
}
#endif

// make sure to include olectl.h for the IPicture interface

#ifndef HIMETRIC_INCH
#define HIMETRIC_INCH 2540
#endif

inline void HIMETRICtoDP(SIZE* pSize)
{
	static int dpiX = 0, dpiY = 0;
	if (dpiX == 0)
	{
		HDC hDC = GetDC(NULL);
		dpiX = GetDeviceCaps(hDC, LOGPIXELSX);
		dpiY = GetDeviceCaps(hDC, LOGPIXELSY);
		ReleaseDC(NULL, hDC);
	}

	pSize->cx = MulDiv(pSize->cx, dpiX, HIMETRIC_INCH);
	pSize->cy = MulDiv(pSize->cy, dpiY, HIMETRIC_INCH);
}

inline void DPtoHIMETRIC(SIZE* pSize)
{
	static int dpiX = 0, dpiY = 0;
	if (dpiX == 0)
	{
		HDC hDC = GetDC(NULL);
		dpiX = GetDeviceCaps(hDC, LOGPIXELSX);
		dpiY = GetDeviceCaps(hDC, LOGPIXELSY);
		ReleaseDC(NULL, hDC);
	}

	pSize->cx = MulDiv(pSize->cx, HIMETRIC_INCH, dpiX);
	pSize->cy = MulDiv(pSize->cy, HIMETRIC_INCH, dpiY);
}

#if defined(__wincodec_h__) && defined(__RMEMORYDC_H__) && defined(__RCOMPTR_H__)
inline bool LoadImage(const BYTE *pData, INT_PTR nDataLen, RMemoryDC &mdc, int cx = -1, int cy = -1)
{
	// Try to use the Windows Imaging Component

	RComPtr<IWICImagingFactory> pImgFac;
	if (SUCCEEDED(pImgFac.CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER)))
	{
		RComPtr<IWICStream> pStream;
		RComPtr<IWICBitmapDecoder> pDecoder;
		RComPtr<IWICBitmapFrameDecode> pFrame;
		RComPtr<IWICFormatConverter> pConverter;
		UINT cxImg, cyImg;

		if (FAILED(pImgFac->CreateStream(&pStream)) ||
				FAILED(pStream->InitializeFromMemory((BYTE*)pData, (DWORD)nDataLen)) ||
				FAILED(pImgFac->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)) ||
				FAILED(pDecoder->GetFrame(0, &pFrame)) ||
				FAILED(pImgFac->CreateFormatConverter(&pConverter)) ||
				FAILED(pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, NULL, 
						0.0, WICBitmapPaletteTypeCustom)) ||
				FAILED(pFrame->GetSize(&cxImg, &cyImg)))
			{ASSERT(false); return false;}

		if (cx == -1 && cy == -1)
			{cx = (int)cxImg; cy = (int)cyImg;}
		else if (cx == -1)
			cx = roundf((float)(cy * cxImg) / cyImg);
		else if (cy == -1)
			cy = roundf((float)(cx * cyImg) / cxImg);

		mdc.Create(cx, cy);
		WICRect rc = {0, 0, cx, cy};

		if (cx == (int)cxImg && cy == (int)cyImg)
			return SUCCEEDED(pConverter->CopyPixels(&rc, cx * 4, cx * cy * 4, (BYTE*)mdc.GetPixels()));

		RComPtr<IWICBitmapScaler> pScaler;
		return (SUCCEEDED(pImgFac->CreateBitmapScaler(&pScaler)) &&
				SUCCEEDED(pScaler->Initialize(pConverter, cx, cy, WICBitmapInterpolationModeFant)) &&
				SUCCEEDED(pScaler->CopyPixels(&rc, cx * 4, cx * cy * 4, (BYTE*)mdc.GetPixels())));
	}

	// ----- Alternative (slower) procedure -----

	// Copy data into global memory
	
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_NODISCARD, nDataLen);
	if (!hMem)
		{mdc.Destroy(); {ASSERT(false); return false;}}
	BYTE *pBuffer = (BYTE*)GlobalLock(hMem);
	memcpy(pBuffer, pData, nDataLen);
	GlobalUnlock(hMem);

	// Create IStream object on global memory (it will free the memory on release)

	IStream *pStream = NULL;
	if (FAILED(CreateStreamOnHGlobal(hMem, TRUE, &pStream)))
		{GlobalFree(hMem); mdc.Destroy(); {ASSERT(false); return false;}}

	// Create IPicture object from stream

	IPicture *pPicture = NULL;
	if (FAILED(OleLoadPicture(pStream, 0, FALSE, IID_IPicture, (void**)&pPicture))) // this costs the most time
		{pStream->Release(); mdc.Destroy(); {ASSERT(false); return false;}}
	pStream->Release();

	// Get image size in pixels

	SIZE szHM;
	pPicture->get_Width(&szHM.cx);
	pPicture->get_Height(&szHM.cy);
	SIZE szDP = szHM;
	HIMETRICtoDP(&szDP);

	// Create destination DC with the correct size

	if (cx == -1 && cy == -1)
		{cx = szDP.cx; cy = szDP.cy;}
	else if (cx == -1)
		cx = roundf((float)(cy * szDP.cx) / szDP.cy);
	else if (cy == -1)
		cy = roundf((float)(cx * szDP.cy) / szDP.cx);

	if (!mdc.Create(cx, cy))
		{pPicture->Release(); {ASSERT(false); return false;}}

	// Render directly to destination DC if no resizing is required

	if (cx == szDP.cx && cy == szDP.cy)
	{
		if (FAILED(pPicture->Render(mdc, 0, 0, szDP.cx, szDP.cy, 0, szHM.cy, szHM.cx, -szHM.cy, NULL)))
			{pPicture->Release(); mdc.Destroy(); {ASSERT(false); return false;}}
		pPicture->Release();
		return true;
	}

	// First render to temporary DC

	RMemoryDC mdcTemp;
	if (!mdcTemp.Create(szDP.cx, szDP.cy))
		{pPicture->Release(); mdc.Destroy(); {ASSERT(false); return false;}}
	if (FAILED(pPicture->Render(mdcTemp, 0, 0, szDP.cx, szDP.cy, 0, szHM.cy, szHM.cx, -szHM.cy, NULL)))
		{pPicture->Release(); mdc.Destroy(); {ASSERT(false); return false;}}
	pPicture->Release();

	// Resize to destination DC

	Resize(&mdc, &mdcTemp, false);
	
	return true;
}

inline bool LoadImage(const RArray<BYTE> &data, RMemoryDC &mdc, int cx = -1, int cy = -1)
	{return LoadImage(data.GetData(), data.GetSize(), mdc, cx, cy);}

inline bool LoadImage(const TCHAR *lpszFilePath, RMemoryDC &mdc, int cx = -1, int cy = -1)
{
	RArray<BYTE> data;
	if (!FileToData(lpszFilePath, data))
		return false;
	if (!LoadImage(data, mdc, cx, cy))
		return false;
	return true;
}
#endif

inline bool LoadIcon(WORD iconID, RMemoryDC *pMemDC)
{
	if (iconID == 0 || !pMemDC)
		ASSERTRETURN(false);

	HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(iconID), IMAGE_ICON, 0, 0, 0);
	if (!hIcon)
		ASSERTRETURN(false);

	ICONINFO ii;
	VERIFY(GetIconInfo(hIcon, &ii));
	if (!ii.hbmColor)
		{DestroyIcon(hIcon); ASSERTRETURN(false);}

	BITMAP bmp;
	VERIFY(GetObject(ii.hbmColor, sizeof(BITMAP), &bmp));
	pMemDC->Create(bmp.bmWidth, bmp.bmHeight);
	DrawIconEx(*pMemDC, 0, 0, hIcon, bmp.bmWidth, bmp.bmHeight, 0, NULL, DI_NORMAL);

	DestroyIcon(hIcon);
	return true;
}

//inline RMemoryDC LoadIcon(WORD iconID)
//	{RMemoryDC mdc; VERIFY(LoadIcon(iconID, &mdc)); return mdc;}
