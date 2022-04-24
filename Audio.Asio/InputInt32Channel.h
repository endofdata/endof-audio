#pragma once

#include <InputOfTSampleChannel.h>
#include <SampleConversion.h>
#include <asio.h>

namespace Audio
{
	namespace Asio
	{
		class InputInt32Channel : public Audio::Foundation::Templates::InputOfTSampleChannel<int, ASIOSTInt32LSB>
		{
		public:
			InputInt32Channel(int iAsioChannel, int* pBufferA, int* pBufferB, int sampleCount) :
				InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount)
			{
			}

		private:
			inline float ReadSample(int*& pSource)
			{
				return Audio::Foundation::SampleConversion::Int32ToFloat(*pSource++);
			}
		};
	}
}
