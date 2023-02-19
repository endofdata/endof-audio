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
			class OutputInt32ChannelPair : public OutputOfTSampleChannelPair<int, Int32LSB, sizeof(int)>
			{
			public:
				OutputInt32ChannelPair(int iAsioChannelLeft, int* pBufferLeftA, int* pBufferLeftB,
					int iAsioChannelRight, int* pBufferRightA, int* pBufferRightB, int sampleCount) :
					OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
						iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount)
				{
				}

			private:
				inline void WriteSample(sample value, int*& pTarget)
				{
					*pTarget++ = SampleConversion::SampleToInt32(value);
				}
			};
		}
	}
}
