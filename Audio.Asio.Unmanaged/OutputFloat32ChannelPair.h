#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <OutputOfTSampleChannelPair.h>
#include <asio.h>

using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Asio
	{
		class OutputFloat32ChannelPair : public OutputOfTSampleChannelPair<float, ASIOSTFloat32LSB, sizeof(float)>
		{
		public:
			OutputFloat32ChannelPair(int iAsioChannelLeft, float* pBufferLeftA, float* pBufferLeftB,
				int iAsioChannelRight, float* pBufferRightA, float* pBufferRightB, int sampleCount) :
				OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
					iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount)
			{
			}

		private:
			inline void WriteSample(sample value, float*& pTarget)
			{
				*pTarget++ = value;
			}
		};
	}
}
