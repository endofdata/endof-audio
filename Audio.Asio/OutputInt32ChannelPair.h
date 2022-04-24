#pragma once

#include <OutputOfTSampleChannelPair.h>
#include <SampleConversion.h>
#include <asio.h>

namespace Audio
{
	namespace Asio
	{
		class OutputInt32ChannelPair : public Audio::Foundation::Templates::OutputOfTSampleChannelPair<int, ASIOSTInt32LSB, sizeof(int)>
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
				*pTarget++ = Audio::Foundation::SampleConversion::SaturatedConvert32(value, 1.0f);
			}
		};
	}
}
