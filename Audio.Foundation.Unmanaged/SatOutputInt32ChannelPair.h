#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <OutputOfTSampleChannelPair.h>
#include <SampleConversionUnmanaged.h>
#include <SampleType.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SatOutputInt32ChannelPair : public OutputOfTSampleChannelPair<int, Int32LSB, sizeof(int)>
			{
			public:
				SatOutputInt32ChannelPair(int iAsioChannelLeft, int* pBufferLeftA, int* pBufferLeftB,
					int iAsioChannelRight, int* pBufferRightA, int* pBufferRightB, int sampleCount, float saturation) :
					OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
						iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount),
					m_saturation(saturation)
				{
				}

			private:
				inline void WriteSample(Sample value, int*& pTarget)
				{
					*pTarget++ = SampleConversion::SampleToSaturatedInt32(value, m_saturation);
				}

				float m_saturation;
			};
		}
	}
}
