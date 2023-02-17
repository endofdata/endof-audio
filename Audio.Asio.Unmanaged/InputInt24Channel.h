#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <InputOfTSampleChannel.h>
#include <SampleConversionUnmanaged.h>
#include <asio.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Templates;


namespace Audio
{
	namespace Asio
	{
		class InputInt24Channel : public InputOfTSampleChannel<byte, ASIOSTInt24LSB>
		{
		public:
			InputInt24Channel(int iAsioChannel, byte* pBufferA, byte* pBufferB, int sampleCount) :
				InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount)
			{
			}

		private:
			inline sample ReadSample(byte*& pSource)
			{
				int unpacked = 0;

				for (int i = 0; i < 3; i++)
				{
					unpacked <<= 8;
					unpacked += *pSource++;
				}
				
				return SampleConversion::Int32ToSample(unpacked);
			}
		};
	}
}
