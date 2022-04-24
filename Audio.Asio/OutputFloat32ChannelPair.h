#pragma once

#include <OutputOfTSampleChannelPair.h>
#include <asio.h>

namespace Audio
{
	namespace Asio
	{
		class OutputFloat32ChannelPair : public Audio::Foundation::Templates::OutputOfTSampleChannelPair<float, ASIOSTFloat32LSB, sizeof(float)>
		{
		public:
			OutputFloat32ChannelPair(int iAsioChannelLeft, float* pBufferLeftA, float* pBufferLeftB,
				int iAsioChannelRight, float* pBufferRightA, float* pBufferRightB, int sampleCount) :
				OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
					iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount)
			{
			}

		private:
			inline void WriteSample(float value, float*& pTarget)
			{
				*pTarget++ = value;
			}
		};
	}
}
