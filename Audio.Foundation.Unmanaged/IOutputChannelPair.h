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
				__interface ISampleReceiver;

				__interface IOutputChannelPair
				{
					_declspec(property(get = get_AsSampleReceiver)) ISampleReceiver& AsSampleReceiver;

					ISampleReceiver& get_AsSampleReceiver();

					_declspec(property(get = get_SampleType)) int SampleType;

					int get_SampleType() = 0;

					void Swap(bool readSecondHalf) = 0;
					void DirectOut(void* pBuffer, bool fLeft, bool fRight) = 0;
				};
			}
		}
	}
}
