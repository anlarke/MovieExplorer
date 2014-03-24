#pragma once

#if WINVER < 0x0601 // only required when compiling for earlier version than Windows 7

// Class declaration

class User32Dll
{
public:
	User32Dll();
	~User32Dll();
};

// Declarations copied from WinUser.h

#define WM_TOUCH	0x0240

DECLARE_HANDLE(HTOUCHINPUT);

typedef struct tagTOUCHINPUT {
    LONG x;
    LONG y;
    HANDLE hSource;
    DWORD dwID;
    DWORD dwFlags;
    DWORD dwMask;
    DWORD dwTime;
    ULONG_PTR dwExtraInfo;
    DWORD cxContact;
    DWORD cyContact;
} TOUCHINPUT, *PTOUCHINPUT;
typedef TOUCHINPUT const * PCTOUCHINPUT;

#define TOUCH_COORD_TO_PIXEL(l)         ((l) / 100)

#define TOUCHEVENTF_MOVE            0x0001
#define TOUCHEVENTF_DOWN            0x0002
#define TOUCHEVENTF_UP              0x0004
#define TOUCHEVENTF_INRANGE         0x0008
#define TOUCHEVENTF_PRIMARY         0x0010
#define TOUCHEVENTF_NOCOALESCE      0x0020
#define TOUCHEVENTF_PEN             0x0040
#define TOUCHEVENTF_PALM            0x0080

#define TOUCHINPUTMASKF_TIMEFROMSYSTEM  0x0001
#define TOUCHINPUTMASKF_EXTRAINFO       0x0002
#define TOUCHINPUTMASKF_CONTACTAREA     0x0004

#define TWF_FINETOUCH       (0x00000001)
#define TWF_WANTPALM        (0x00000002)

// Function declarations

BOOL GetTouchInputInfo(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize);
BOOL CloseTouchInputHandle(HTOUCHINPUT hTouchInput);
BOOL RegisterTouchWindow(HWND hwnd, ULONG ulFlags);
BOOL UnregisterTouchWindow(HWND hwnd);
BOOL IsTouchWindow(HWND hwnd, PULONG pulFlags);

#endif // WINVER < 0x0601
