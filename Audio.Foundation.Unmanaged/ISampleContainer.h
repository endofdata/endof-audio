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
				__interface ISampleBuffer;

				__interface ISampleContainer
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
