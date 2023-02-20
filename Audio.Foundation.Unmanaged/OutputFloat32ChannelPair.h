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
			class OutputFloat32ChannelPair : public OutputOfTSampleChannelPair<float, Float32LSB, sizeof(float)>
			{
			public:
				OutputFloat32ChannelPair(int iAsioChannelLeft, float* pBufferLeftA, float* pBufferLeftB,
					int iAsioChannelRight, float* pBufferRightA, float* pBufferRightB, int sampleCount) :
					OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
						iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount)
				{
				}

			private:
				inline void WriteSample(Sample value, float*& pTarget)
				{
					*pTarget++ = SampleConversion::SampleToFloat32(value);
				}
			};
		}
	}
}
