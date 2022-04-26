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
				__interface ISampleReceiver;

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("5d96f162-5569-40b6-9cc1-529800d1eaa2")) IOutputChannelPair : public IUnknown
				{
					_declspec(property(get = get_SampleType)) int SampleType;

					int get_SampleType() = 0;

					void Swap(bool readSecondHalf) = 0;
					void DirectOut(void* pBuffer, bool fLeft, bool fRight) = 0;
				};
			}
		}
	}
}
