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
			class SatInputInt24Channel : public InputOfTSampleChannel<byte, Int24LSB>
			{
			public:
				SatInputInt24Channel(int iAsioChannel, byte* pBufferA, byte* pBufferB, int sampleCount, float saturation = 1.0f) :
					InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount),
					m_saturation(saturation)
				{
				}

			private:
				inline Sample ReadSample(byte*& pSource)
				{
					int unpacked = 0;

					for (int i = 0; i < 3; i++)
					{
						unpacked <<= 8;
						unpacked += *pSource++;
					}

					return SampleConversion::Int32ToSaturatedSample(unpacked, m_saturation);
				}

				float m_saturation;
			};
		}
	}
}
