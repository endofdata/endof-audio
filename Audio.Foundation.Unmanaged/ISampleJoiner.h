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

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("519e9d40-db0b-42c4-b435-90d7ec67206a")) ISampleJoiner : public IUnknown
				{
					_declspec(property(get = get_OutputLink, put = put_OutputLink)) IChannelLink* OutputLink;

					IChannelLink* get_OutputLink() = 0;
					void put_OutputLink(IChannelLink* value) = 0;

					void Flush() = 0;

					void Send() = 0;

					void MixInput(float* pSourceLeft, float* pSourceRight, float level, float pan) = 0;
				};
			}
		}
	}
}
