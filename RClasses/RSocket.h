#ifndef __RSOCKET_H__
#define __RSOCKET_H__

//
// Remember to link against:
// #pragma comment(lib, "Ws2_32.lib") // can also be used in a 64-bit build (despite its name)
//
// For connect timeout see: http://www.codeguru.com/forum/showthread.php?t=312668
// or use overlapped socket.
//

#include <winsock2.h>
#include <ws2tcpip.h>

#define SOCKET_INIT()		WSADATA wsaData; WSAStartup(MAKEWORD(2, 2), &wsaData)
#define SOCKET_UNINIT()		WSACleanup()

class RSocket
{
public:
	RSocket() : m_socket(INVALID_SOCKET)
	{
	}

	~RSocket()
	{
		Close();
	}

	bool Connect(const TCHAR *lpszAddress, INT_PTR nPort)
	{
		Close();

		// Convert to multi-byte character strings

		int nLen = (int)_tcslen(lpszAddress);
		int nLenMB = WideCharToMultiByte(CP_ACP, 0, lpszAddress, nLen, NULL, 0, NULL, NULL);
		char *lpszAddressMB = new char[nLenMB+1];
		if (WideCharToMultiByte(CP_ACP, 0, lpszAddress, nLen, lpszAddressMB, nLenMB, NULL, NULL) != nLenMB)
			{delete[] lpszAddressMB; ASSERT(false); return false;}
		lpszAddressMB[nLenMB] = 0;

		char *lpszPortMB = new char[32];
		if (sprintf(lpszPortMB, "%d", (int)nPort) == -1)
			{delete[] lpszAddressMB; delete[] lpszPortMB; ASSERT(false); return false;}

		// Resolve address

		addrinfo hints;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		addrinfo *pResult;
		if (getaddrinfo(lpszAddressMB, lpszPortMB, &hints, &pResult) != 0)
			{delete[] lpszAddressMB; delete[] lpszPortMB; return false;}

		delete[] lpszAddressMB;
		delete[] lpszPortMB;

		// Create compatible socket

		m_socket = socket(pResult->ai_family, pResult->ai_socktype, pResult->ai_protocol);
		if (m_socket == INVALID_SOCKET)
			{freeaddrinfo(pResult); ASSERT(false); return false;}

		// Set receive time-out

		DWORD dwTimeOut = 2000; // 2 sec
		setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTimeOut, sizeof(DWORD));

		// Attempt to connect to resolved address(es)

		for (addrinfo *p = pResult; p; p = p->ai_next)
			if (connect(m_socket, p->ai_addr, (int)p->ai_addrlen) != SOCKET_ERROR)
				{freeaddrinfo(pResult); return true;}

		freeaddrinfo(pResult);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		return false;
	}

	void Close()
	{
		if (m_socket != INVALID_SOCKET)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
	}

	bool Send(const BYTE *pData, INT_PTR nDataLen, bool bDone = true)
	{
		if (m_socket == INVALID_SOCKET)
			ASSERTRETURN(false);

		if (send(m_socket, (const char*)pData, (int)nDataLen, 0) != SOCKET_ERROR)
		{
			if (bDone)
				if (shutdown(m_socket, SD_SEND) == SOCKET_ERROR)
					ASSERTRETURN(false);
			return true;
		}
		ASSERTRETURN(false);
	}

	INT_PTR Receive(BYTE *pBuf, INT_PTR nBufLen)
	{
		if (m_socket == INVALID_SOCKET)
			{ASSERT(false); return -1;}

		INT_PTR n = recv(m_socket, (char*)pBuf, (int)nBufLen, 0);
		if (n != SOCKET_ERROR)
			return n;

		TRACE(_T("RSocket::Receive failed WSAGetLastError() = %d (thread 0x%.3x)\n"), 
				WSAGetLastError(), GetCurrentThreadId());
		//ASSERT(false);
		return -1;
	}

	operator SOCKET() const
	{
		return m_socket;
	}

protected:
	SOCKET m_socket;
};

#endif // __RSOCKET_H__
