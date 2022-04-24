#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface IChannelLink;

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("3a7970a7-fdb6-4dc6-ba80-540ab339c988")) ISampleReceiver : public IUnknown
				{
					void Flush() = 0;

					void Receive(IChannelLink& channelLink) = 0;
				};
			}
		}
	}
}
