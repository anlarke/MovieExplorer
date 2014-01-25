#pragma once

#define SCALE_INIT()	ScaleInit()
#define SCX(n)			ScaleX(n)
#define SCY(n)			ScaleY(n)
#define _S(n)			ScaleX(n)
#define S(n)			ScaleX(n)

void ScaleInit();
int ScaleX(int n);
int ScaleY(int n);
float GetScale();
void SetScale(float f);
float GetSystemScale();


// These function are called so often I've decided to optimize and put them in a .cpp file.

/*
#pragma once

#define SCX(n)	ScaleX(n)
#define SCY(n)	ScaleY(n)

inline void ScaleCore(int* pnScaleX, int* pnScaleY, float* pfGetScale, float* pfSetScale, float* pfGetSystemScale)
{
	static float fSystemScaleX = 0.0f;
	static float fSystemScaleY = 0.0f;

	if (fSystemScaleX == 0.0f)
	{
		HDC hDC = GetDC(GetDesktopWindow());
		fSystemScaleX = GetDeviceCaps(hDC, LOGPIXELSX) / 96.0f;
		fSystemScaleY = GetDeviceCaps(hDC, LOGPIXELSY) / 96.0f;
		ReleaseDC(GetDesktopWindow(), hDC);
	}
	
	static float fScaleX = fSystemScaleX;
	static float fScaleY = fSystemScaleY;

	if (pnScaleX)
	{
		if (fScaleX == 1.0f)
			return;

		if (*pnScaleX != 0)
		{
			int n = (int)(*pnScaleX * fScaleX);
			*pnScaleX = (n == 0 ? 1 : n);
		}
	}
	else if (pnScaleY)
	{
		if (fScaleY == 1.0f)
			return;

		if (*pnScaleY != 0)
		{
			int n = (int)(*pnScaleY * fScaleY);
			*pnScaleY = (n == 0 ? 1 : n);
		}
	}
	else if (pfGetScale)
	{
		*pfGetScale = fScaleX;
	}
	else if (pfSetScale)
	{
		fScaleX = *pfSetScale;
		fScaleY = *pfSetScale * fSystemScaleY / fSystemScaleX;
	}
	else if (pfGetSystemScale)
	{
		*pfGetSystemScale = fSystemScaleX;
	}
}

inline int ScaleX(int n) {ScaleCore(&n, NULL, NULL, NULL, NULL); return n;}
inline int ScaleY(int n) {ScaleCore(NULL, &n, NULL, NULL, NULL); return n;}
inline float GetScale() {float f; ScaleCore(NULL, NULL, &f, NULL, NULL); return f;}
inline void SetScale(float f) {ScaleCore(NULL, NULL, NULL, &f, NULL);}
inline float GetSystemScale() {float f; ScaleCore(NULL, NULL, NULL, NULL, &f); return f;}
*/
