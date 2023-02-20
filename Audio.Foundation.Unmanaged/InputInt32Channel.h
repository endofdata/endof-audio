#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <InputOfTSampleChannel.h>
#include <SampleConversionUnmanaged.h>
#include <SampleType.h>

using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class InputInt32Channel : public InputOfTSampleChannel<int, Int32LSB>
			{
			public:
				InputInt32Channel(int iAsioChannel, int* pBufferA, int* pBufferB, int sampleCount) :
					InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount)
				{
				}

			private:
				inline Sample ReadSample(int*& pSource)
				{
					return SampleConversion::Int32ToSample(*pSource++);
				}
			};
		}
	}
}
