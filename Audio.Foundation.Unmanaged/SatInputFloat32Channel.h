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
			class SatInputFloat32Channel : public InputOfTSampleChannel<float, Float32LSB>
			{
			public:
				SatInputFloat32Channel(int iAsioChannel, float* pBufferA, float* pBufferB, int sampleCount, float saturation = 1.0f) :
					InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount),
					m_saturation(saturation)
				{
				}

			private:
				inline Sample ReadSample(float*& pSource)
				{
					return SampleConversion::Float32ToSaturatedSample(*pSource++, m_saturation);
				}

				float m_saturation;
			};
		}
	}
}
