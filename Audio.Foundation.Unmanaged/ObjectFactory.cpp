#include "pch.h"
#include "ObjectFactory.h"
#include "MeterChannel.h"
#include "SampleBuffer.h"
#include "SampleContainer.h"
#include "SampleJoiner.h"
#include "SampleSharer.h"
#include "FileWriter.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

// static
ISampleBufferPtr ObjectFactory::CreateSampleBuffer(int sampleCount)
{
	return new SampleBuffer(sampleCount);
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

// static
ISampleJoinerPtr ObjectFactory::CreateSampleJoiner(int sampleCount, int channelCount)
{
	return new SampleJoiner(sampleCount, channelCount);
}

// static
IMeterChannelPtr ObjectFactory::CreateMeterChannel(int sampleRate, int channelCount)
{
	return new MeterChannel(sampleRate, channelCount);
}

// static 
ISampleReceiverPtr ObjectFactory::CreateSampleReceiver(const std::string& filename)
{
	return new FileWriter(filename);
}
