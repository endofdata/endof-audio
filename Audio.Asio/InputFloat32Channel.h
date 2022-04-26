#pragma once

#include <InputOfTSampleChannel.h>
#include <SampleConversionUnmanaged.h>
#include <asio.h>

using namespace Audio::Foundation::Unmanaged::Templates;


namespace Audio
{
	namespace Asio
	{
		class InputFloat32Channel : public InputOfTSampleChannel<float, ASIOSTFloat32LSB>
		{
		public:
			InputFloat32Channel(int iAsioChannel, float* pBufferA, float* pBufferB, int sampleCount) :
				InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount)
			{
			}

		private:
			inline float ReadSample(float*& pSource)
			{
				return *pSource++;
			}
		};
	}
}
