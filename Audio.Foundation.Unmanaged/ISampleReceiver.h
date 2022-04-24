#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface IChannelLink;

				__interface ISampleReceiver
				{
					void Flush() = 0;

					void Receive(IChannelLink& channelLink) = 0;
				};
			}
		}
	}
}
