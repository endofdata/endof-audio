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
			class InputFloat32Channel : public InputOfTSampleChannel<float, Float32LSB>
			{
			public:
				InputFloat32Channel(int iAsioChannel, float* pBufferA, float* pBufferB, int sampleCount) :
					InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount)
				{
				}

			private:
				inline sample ReadSample(float*& pSource)
				{
					return SampleConversion::Float32ToSample(*pSource++);
				}
			};
		}
	}
}
