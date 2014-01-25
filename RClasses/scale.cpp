#include "stdafx.h"
#include "scale.h"

float g_fSystemScaleX = 0.0f;
float g_fSystemScaleY = 0.0f;
float g_fScaleX = 0.0f;
float g_fScaleY = 0.0f;

void ScaleInit()
{
	HDC hDC = GetDC(GetDesktopWindow());
	g_fSystemScaleX = GetDeviceCaps(hDC, LOGPIXELSX) / 96.0f;
	g_fSystemScaleY = GetDeviceCaps(hDC, LOGPIXELSY) / 96.0f;
	g_fScaleX = g_fSystemScaleX;
	g_fScaleY = g_fSystemScaleY;
	ReleaseDC(GetDesktopWindow(), hDC);
}

int ScaleX(int n)
{
	ASSERT(g_fScaleX != 0.0);
	if (g_fScaleX == 1.0f || n == 0)
		return n;
	n = (int)(n * g_fScaleX);
	return (n == 0 ? 1 : n);
}

int ScaleY(int n)
{
	ASSERT(g_fScaleY != 0.0);
	if (g_fScaleY == 1.0f || n == 0)
		return n;
	n = (int)(n * g_fScaleY);
	return (n == 0 ? 1 : n);
}

float GetScale()
{
	ASSERT(g_fScaleX != 0.0);
	return g_fScaleX;
}

void SetScale(float f)
{
	g_fScaleX = f;
	g_fScaleY = (f * g_fSystemScaleY) / g_fSystemScaleX;
}

float GetSystemScale()
{
	ASSERT(g_fSystemScaleX != 0.0);
	return g_fSystemScaleX;
}
