#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface IChannelLink;
				__interface ISampleContainer;
				__interface ISampleReceiver;

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("e8ed7980-1d09-46b0-9c9c-df015a1f72fe")) ISampleSharer
				{
					_declspec(property(get = get_Send)) IChannelLink* Send[];

					IChannelLink* get_Send(int iIdx) = 0;

					void AddSend(ISampleContainer& fromChannel, ISampleReceiver& toChannel, float volume, float pan) = 0;
					void RemoveSend(ISampleReceiver& toChannel) = 0;
					void RemoveAllSends() = 0;

					void RouteToSends() = 0;
				};

			}
		}
	}
}
