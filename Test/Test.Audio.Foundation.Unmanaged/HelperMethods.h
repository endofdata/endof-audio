#pragma once

#include <ISampleBuffer.h>
#include <ISampleContainer.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Test
{
	namespace Audio
	{
		namespace Foundation
		{
			namespace Unmanaged
			{
				enum class Key
				{
					C = 0,
					CIS,
					D,
					DIS,
					E,
					F,
					FIS,
					G,
					GIS,
					A,
					AIS,
					B
				};

				class HelperMethods
				{
				public:
					static const double SAMPLE_RATE;

					static void TestSampleBuffer(ISampleBufferPtr pBuffer, int sampleCount);

					static void TestSampleContainer(ISampleContainerPtr pContainer, int sampleCount, int channelCount);

					static ISampleContainerPtr HelperMethods::CreateTestContainer(int sampleCount, int channelCount, 
						const Key key[], double sampleRate = SAMPLE_RATE);

				private:
					static double Frequencies[];
				};
			}
		}
	}
}
