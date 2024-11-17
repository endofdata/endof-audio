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
			class SatInputInt32Channel : public InputOfTSampleChannel<int, Int32LSB>
			{
			public:
				SatInputInt32Channel(int iAsioChannel, int* pBufferA, int* pBufferB, int sampleCount, float saturation = 1.0f) :
					InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount),
					m_saturation(saturation)
				{
				}

			private:
				inline Sample ReadSample(int*& pSource)
				{
					return SampleConversion::Int32ToSaturatedSample(*pSource++, m_saturation);
				}

				float m_saturation;
			};
		}
	}
}
