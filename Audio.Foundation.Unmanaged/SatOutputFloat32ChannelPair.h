#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <OutputOfTSampleChannelPair.h>
#include <SampleConversionUnmanaged.h>
#include <SampleType.h>


using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SatOutputFloat32ChannelPair : public OutputOfTSampleChannelPair<float, Float32LSB, sizeof(float)>
			{
			public:
				SatOutputFloat32ChannelPair(int iAsioChannelLeft, float* pBufferLeftA, float* pBufferLeftB,
					int iAsioChannelRight, float* pBufferRightA, float* pBufferRightB, int sampleCount, float saturation) :
					OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
						iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount),
					m_saturation(saturation)
				{
				}

			private:
				inline void WriteSample(Sample value, float*& pTarget)
				{
					*pTarget++ = SampleConversion::SampleToSaturatedFloat32(value, m_saturation);
				}

				float m_saturation;
			};
		}
	}
}
