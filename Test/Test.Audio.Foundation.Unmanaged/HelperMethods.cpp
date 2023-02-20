#include "pch.h"
#include "HelperMethods.h"
#include "CppUnitTest.h"
#include <ObjectFactory.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Foundation::Unmanaged;

using namespace Test::Audio::Foundation::Unmanaged;

void HelperMethods::TestSampleBuffer(ISampleBufferPtr pBuffer, int sampleCount)
{
	Assert::AreEqual(sampleCount, pBuffer->SampleCount, L"Buffer sample count is OK");

	for (int s = 0; s < sampleCount; s++)
	{
		sample testValue = (sample)s / 12.0f;
		pBuffer->Samples[s] = testValue;

		Assert::AreEqual(testValue, pBuffer->Samples[s], L"Buffer read/write retains value");
	}
	Assert::IsNotNull(pBuffer->SamplePtr, L"Buffer pointer is not nullptr");
}

void HelperMethods::TestSampleContainer(ISampleContainerPtr pContainer, int sampleCount, int channelCount)
{
	Assert::AreEqual(sampleCount, pContainer->SampleCount, L"Container sample count is OK");
	Assert::AreEqual(channelCount, pContainer->ChannelCount, L"Container channel count is OK");

	for (int c = 0; c < channelCount; c++)
	{
		HelperMethods::TestSampleBuffer(pContainer->Channels[c], sampleCount);
	}
}

ISampleContainerPtr HelperMethods::CreateTestContainer(int sampleCount, int channelCount)
{
	ISampleContainerPtr container = ObjectFactory::CreateSampleContainer(sampleCount, channelCount);

	for (int c = 0; c < channelCount; c++)
	{
		sample* pSamples = container->Channels[c]->SamplePtr;

		for (int s = 0; s < sampleCount; s++)
		{
			*pSamples++ = (sample)sin((double)s / 100.0);
		}
	}
	return container;
}
