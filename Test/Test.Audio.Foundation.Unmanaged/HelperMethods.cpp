#include "pch.h"
#include "HelperMethods.h"
#include "CppUnitTest.h"
#include <FoundationObjectFactory.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Foundation::Unmanaged;

using namespace Test::Audio::Foundation::Unmanaged;

const double HelperMethods::SAMPLE_RATE = 48000.0;

double HelperMethods::Frequencies[] =
{
	261.6, 277.2, 293.7, 311.1, 329.6, 349.2, 370, 392, 415.3, 440, 466.2, 493.9
};

void HelperMethods::TestSampleBuffer(ISampleBufferPtr pBuffer, int sampleCount)
{
	Assert::AreEqual(sampleCount, pBuffer->SampleCount, L"Buffer sample count is OK");

	for (int s = 0; s < sampleCount; s++)
	{
		Sample testValue = (Sample)s / 12.0f;
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

ISampleContainerPtr HelperMethods::CreateTestContainer(int sampleCount, int channelCount, const Key key[], double sampleRate)
{
	ISampleContainerPtr container = FoundationObjectFactory::CreateSampleContainer(sampleCount, channelCount);

	for (int c = 0; c < channelCount; c++)
	{
		Sample* pSamples = container->Channels[c]->SamplePtr;
		int freqIndex = static_cast<int>(key[c]);
		if (freqIndex < 0 || freqIndex >= _countof(Frequencies))
		{
			throw std::invalid_argument("all 'key' values must be in the range of 0 to 11.");
		}

		double step = sampleRate / Frequencies[freqIndex];
		double value = 0;

		for (int s = 0; s < sampleCount; s++)
		{
			*pSamples++ = (Sample)sin(value);
			value += step;
		}
	}
	return container;
}
