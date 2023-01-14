#include "pch.h"
#include "ObjectFactory.h"
#include "MeterChannel.h"
#include "SampleBuffer.h"
#include "SampleContainer.h"
#include "SampleJoiner.h"
#include "SampleSharer.h"
#include "FileWriter.h"
#include "FileSource.h"
#include "Take.h"
#include "Timeline.h"

#include <stdexcept>

int ObjectFactory::LastTakeId = 0;

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
ISampleReceiverPtr ObjectFactory::CreateFileReceiver(const std::string& filename)
{
	return new FileWriter(filename);
}

// static 
ISampleSourcePtr ObjectFactory::CreateFileSource(int sampleCount, int channelCount, const std::string& filename)
{
	return new FileSource(filename, CreateSampleContainer(sampleCount, channelCount));
}

// static
ITakePtr ObjectFactory::CreateTake(ISampleContainerPtr container, Time position, Time length)
{
	return new Take(NextTakeId(), container, position, length);
}

// static 
ITimelinePtr ObjectFactory::CreateTimeline()
{
	return new Timeline();
}

int ObjectFactory::NextTakeId()
{
	// HACK: This could overflow after a few billions of added takes
	return ++LastTakeId;
}
