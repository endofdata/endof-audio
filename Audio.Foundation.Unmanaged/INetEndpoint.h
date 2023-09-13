#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <AudioTime.h>
#include <comdef.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("B51BA775-063D-4FB2-A2B0-BA8DF3614F7E")) INetEndpoint : public IUnknown
				{
					bool get_IsServer() const = 0;
					_declspec(property(get = get_IsServer)) bool IsServer;

					bool get_IsUDP() const = 0;
					_declspec(property(get = get_IsUDP)) bool IsUDP;

					bool get_IsIPv6() const = 0;
					_declspec(property(get = get_IsIPv6)) bool IsIPv6;

					int get_BufferSize() const = 0;
					_declspec(property(get = get_BufferSize)) int BufferSize;

					bool get_IsOpen() const = 0;
					_declspec(property(get = get_IsOpen)) bool IsOpen;

					void Open(const wchar_t* host, unsigned short port) = 0;
					void Close() = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(INetEndpoint, __uuidof(INetEndpoint));
			}
		}
	}
}