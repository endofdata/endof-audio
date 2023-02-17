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
				class HelperMethods
				{
				public:
					static void TestSampleBuffer(ISampleBufferPtr pBuffer, int sampleCount);

					static void TestSampleContainer(ISampleContainerPtr pContainer, int sampleCount, int channelCount);

					static ISampleContainerPtr HelperMethods::CreateTestContainer(int sampleCount, int channelCount);
				};
			}
		}
	}
}
