#pragma once

#include <InputOfTSampleChannel.h>

namespace Audio
{
	namespace Asio
	{
		class InputFloat32Channel : public Audio::Foundation::Templates::InputOfTSampleChannel<float, ASIOSTFloat32LSB>
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
