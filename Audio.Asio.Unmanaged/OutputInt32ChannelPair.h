#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <OutputOfTSampleChannelPair.h>
#include <SampleConversionUnmanaged.h>
#include <asio.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Asio
	{
		class OutputInt32ChannelPair : public OutputOfTSampleChannelPair<int, ASIOSTInt32LSB, sizeof(int)>
		{
		public:
			OutputInt32ChannelPair(int iAsioChannelLeft, int* pBufferLeftA, int* pBufferLeftB,
				int iAsioChannelRight, int* pBufferRightA, int* pBufferRightB, int sampleCount) :
				OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
					iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount)
			{
			}

		private:
			inline void WriteSample(float value, int*& pTarget)
			{
				*pTarget++ = SampleConversion::SaturatedConvert32(value, 1.0f);
			}
		};
	}
}
