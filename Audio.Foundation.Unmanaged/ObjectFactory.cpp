#include "pch.h"
#include "ObjectFactory.h"
#include "MeterChannel.h"
#include "SampleBuffer.h"
#include "SampleContainer.h"
#include "SampleJoiner.h"
#include "SampleSharer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

// static
IMeterChannelPtr ObjectFactory::CreateMeterChannel(int sampleRate, int channelCount)
{
	return new MeterChannel(sampleRate, channelCount);
}

// static
ISampleBufferPtr ObjectFactory::CreateSampleBuffer(int sampleCount)
{
	return new SampleBuffer(sampleCount);
}

// static
ISampleJoinerPtr ObjectFactory::CreateSampleJoiner(int sampleCount, int channelCount)
{
	return new SampleJoiner(sampleCount, channelCount);
}

// static
ISampleContainerPtr ObjectFactory::CreateSampleContainer(int sampleCount, int channelCount)
{
	return new SampleContainer(sampleCount, channelCount);
}

// static
ISampleSharerPtr ObjectFactory::CreateSampleSharer()
{
	return new SampleSharer();
}
