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
				__interface ISampleBuffer;

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("4d5806ba-6683-48e6-93bb-2e6025e00226")) ISampleContainer
				{
					_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

					bool get_IsActive() = 0;
					void put_IsActive(bool value) = 0;

					_declspec(property(get = get_LeftChannel)) ISampleBuffer* LeftChannel;

					ISampleBuffer* get_LeftChannel() = 0;

					_declspec(property(get = get_RightChannel)) ISampleBuffer* RightChannel;

					ISampleBuffer* get_RightChannel() = 0;
				};
			}
		}
	}
}
