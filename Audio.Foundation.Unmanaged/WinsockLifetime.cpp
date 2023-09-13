#include "pch.h"
#include "WinsockLifetime.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <WinSock2.h>

using namespace Audio::Foundation::Unmanaged;

unsigned int WinsockLifetime::s_winSockInitCount = 0;


void WinsockLifetime::InitWinSock()
{
	WSADATA wsaData;

	std::memset(&wsaData, 0, sizeof(WSADATA));

	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (error != 0)
	{
		std::ostringstream builder;

		builder << "Failed to initialize WinSock. Error code 0x" << std::hex << std::setw(8) << std::setfill('0') << HRESULT_FROM_WIN32(error);
		throw std::runtime_error(builder.str());
	}
	InterlockedIncrement(&s_winSockInitCount);
}


void WinsockLifetime::ExitWinSock()
{
	unsigned int currentCount = InterlockedCompareExchange(&s_winSockInitCount, 0, 0);

	if (currentCount > 0)
	{
		WSACleanup();
		InterlockedDecrement(&s_winSockInitCount);
	}
}
