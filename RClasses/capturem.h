#pragma once

#define CAPTUREM_INIT		InitCaptureM
#define CAPTUREM_UNINIT		UninitCaptureM

void InitCaptureM();
void UninitCaptureM();

void SetCaptureM(HWND hWnd);
bool ReleaseCaptureM(HWND hWnd);
