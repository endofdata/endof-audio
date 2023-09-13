#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX						// Exclude min() and max() definitions to use std::min/max without errors
// Windows Header Files
#include <windows.h>
#include <mmeapi.h>
#include <WinSock2.h>
#include <WS2tcpip.h>