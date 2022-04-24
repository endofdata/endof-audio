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
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("7df79f36-35fb-4449-b72d-5952c1f4fdbc")) ISampleBuffer
				{
					void Flush() = 0;

					_declspec(property(get = get_Sample, put = put_Sample)) float Sample[]; //!< Gets or sets the sample at the given index

					float get_Sample(int iIdx) = 0;
					void put_Sample(int iIdx, float value) = 0;

					_declspec(property(get = get_SamplePtr)) float* SamplePtr; //!< Gets the address of the sample buffer

					float* get_SamplePtr() = 0;

					_declspec(property(get = get_SampleCount)) int SampleCount; //!< Gets the maximum number of samples in the buffer

					int get_SampleCount() = 0;
				};
			}
		}
	}
}
