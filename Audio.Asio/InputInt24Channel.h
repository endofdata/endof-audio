#pragma once

#include <InputOfTSampleChannel.h>
#include <SampleConversion.h>
#include <asio.h>

namespace Audio
{
	namespace Asio
	{
		class InputInt24Channel : public Audio::Foundation::Templates::InputOfTSampleChannel<byte, ASIOSTInt24LSB>
		{
		public:
			InputInt24Channel(int iAsioChannel, byte* pBufferA, byte* pBufferB, int sampleCount) :
				InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount)
			{
			}

		private:
			inline float ReadSample(byte*& pSource)
			{
				int unpacked = 0;

				for (int i = 0; i < 3; i++)
				{
					unpacked <<= 8;
					unpacked += *pSource++;
				}
				
				return Audio::Foundation::SampleConversion::Int32ToFloat(unpacked);
			}
		};
	}
}
