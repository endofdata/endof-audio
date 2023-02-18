#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <InputOfTSampleChannel.h>
#include <SampleConversionUnmanaged.h>
#include <SampleType.h>


using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class InputInt24Channel : public InputOfTSampleChannel<byte, Int24LSB>
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
}
