#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <InputOfTSampleChannel.h>
#include <SampleConversionUnmanaged.h>
#include <asio.h>
#include <UnknownBase.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Templates;

namespace Audio
{
	namespace Asio
	{
		class InputInt32Channel : public InputOfTSampleChannel<int, ASIOSTInt32LSB>
		{
		public:
			InputInt32Channel(int iAsioChannel, int* pBufferA, int* pBufferB, int sampleCount) :
				InputOfTSampleChannel(iAsioChannel, pBufferA, pBufferB, sampleCount)
			{
			}
			
		private:
			inline sample ReadSample(int*& pSource)
			{
				return SampleConversion::Int32ToSample(*pSource++);
			}
		};
	}
}
