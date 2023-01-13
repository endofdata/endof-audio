#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <OutputOfTSampleChannelPair.h>
#include <asio.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Asio
	{
		class OutputInt24ChannelPair : public OutputOfTSampleChannelPair<byte, ASIOSTInt24LSB, 3>
		{
		public:
			OutputInt24ChannelPair(int iAsioChannelLeft, byte* pBufferLeftA, byte* pBufferLeftB,
				int iAsioChannelRight, byte* pBufferRightA, byte* pBufferRightB, int sampleCount) :
				OutputOfTSampleChannelPair(iAsioChannelLeft, pBufferLeftA, pBufferLeftB,
					iAsioChannelRight, pBufferRightA, pBufferRightB, sampleCount)
			{
			}

		private:
			inline void WriteSample(float value, byte*& pTarget)
			{
				int unpacked = SampleConversion::SaturatedConvert32(value, 1.0f);

				for (int i = 0; i < 3; i++)
				{
					*pTarget++ = (byte)unpacked;
					unpacked >>= 8;
				}
			}
		};
	}
}
