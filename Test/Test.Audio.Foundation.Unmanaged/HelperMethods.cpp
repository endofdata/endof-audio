#include "pch.h"
#include "HelperMethods.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Foundation::Unmanaged;

using namespace Test::Audio::Foundation::Unmanaged;

void HelperMethods::TestSampleBuffer(ISampleBufferPtr pBuffer, int sampleCount)
{
	Assert::AreEqual(sampleCount, pBuffer->SampleCount, L"Buffer sample count is OK");

	for (int s = 0; s < sampleCount; s++)
	{
		float testValue = (float)s / 12.0f;
		pBuffer->Samples[s] = testValue;

		Assert::AreEqual(testValue, pBuffer->Samples[s], L"Buffer read/write retains value");
	}
	Assert::IsNotNull(pBuffer->SamplePtr, L"Buffer pointer is not nullptr");
}

void HelperMethods::TestSampleContainer(ISampleContainerPtr pContainer, int sampleCount, int channelCount)
{
	Assert::AreEqual(sampleCount, pContainer->SampleCount, L"Container sample count is OK");
	Assert::AreEqual(channelCount, pContainer->ChannelCount, L"Container channel count is OK");
	Assert::IsFalse(pContainer->IsActive, L"Container is not active initially.");

	for (int c = 0; c < channelCount; c++)
	{
		HelperMethods::TestSampleBuffer(pContainer->Channels[c], sampleCount);
	}
}
