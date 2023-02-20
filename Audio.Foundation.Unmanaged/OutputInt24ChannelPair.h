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
			class OutputInt24ChannelPair : public OutputOfTSampleChannelPair<byte, Int24LSB, 3>
			{
			public:
				OutputInt24ChannelPair(int iAsioChannelLeft, byte* pBufferLeftA, byte* pBufferLeftB,
					int iAsioChannelRight, byte* pBufferRightA, byte* pBufferRightB, int sampleCount) :
					OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
						iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount)
				{
				}

			private:
				inline void WriteSample(Sample value, byte*& pTarget)
				{
					int unpacked = SampleConversion::SampleToInt24(value);

					for (int i = 0; i < 3; i++)
					{
						*pTarget++ = (byte)unpacked;
						unpacked >>= 8;
					}
				}
			};
		}
	}
}
