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
#include "InputInt32Channel.h"
#include "InputInt24Channel.h"
#include "InputFloat32Channel.h"
#include "OutputInt32ChannelPair.h"
#include "OutputInt24ChannelPair.h"
#include "OutputFloat32ChannelPair.h"
#include "AudioFoundationException.h"

#include <stdexcept>

int ObjectFactory::LastTakeId = 0;

using namespace Audio::Foundation::Unmanaged;


ISampleBufferPtr ObjectFactory::CreateSampleBuffer(int sampleCount)
{
	return new SampleBuffer(sampleCount);
}

ISampleContainerPtr ObjectFactory::CreateSampleContainer(int sampleCount, int channelCount)
{
	return new SampleContainer(sampleCount, channelCount);
}

ISampleSharerPtr ObjectFactory::CreateSampleSharer()
{
	return new SampleSharer();
}

ISampleJoinerPtr ObjectFactory::CreateSampleJoiner()
{
	return new SampleJoiner();
}

IMeterChannelPtr ObjectFactory::CreateMeterChannel(int sampleRate, int channelCount)
{
	return new MeterChannel(sampleRate, channelCount);
}

ISampleReceiverPtr ObjectFactory::CreateFileReceiver(const std::string& filename)
{
	return new FileWriter(filename);
}

ISampleProcessorPtr ObjectFactory::CreateFileSource(int sampleCount, int channelCount, const std::string& filename)
{
	return new FileSource(filename, CreateSampleContainer(sampleCount, channelCount));
}

ITakePtr ObjectFactory::CreateTake(ISampleContainerPtr container, Time position, Time length)
{
	return new Take(NextTakeId(), container, position, length);
}

ITimelinePtr ObjectFactory::CreateTimeline()
{
	return new Timeline();
}

IInputChannelPtr ObjectFactory::CreateInputChannel(int sampleType, int hwChannelId, void* pHwBufferA, void* pHwBufferB, int sampleCount)
{
	IInputChannelPtr input = nullptr;

	switch (sampleType)
	{
	case Int32LSB:
		input = new InputInt32Channel(hwChannelId, (int*)pHwBufferA, (int*)pHwBufferB, sampleCount);
		break;
	case Int24LSB:
		input = new InputInt24Channel(hwChannelId, (byte*)pHwBufferA, (byte*)pHwBufferB, sampleCount);
		break;
	case Float32LSB:
		input = new InputFloat32Channel(hwChannelId, (float*)pHwBufferA, (float*)pHwBufferB, sampleCount);
		break;
	default:
		throw AudioFoundationException("Unsupported sample type.", E_UNEXPECTED);
	}

	if (nullptr == input)
		throw AudioFoundationException("Not enough memory for InputChannel instance.", E_OUTOFMEMORY);

	return input;
}

IOutputChannelPairPtr ObjectFactory::CreateOutputChannelPair(int sampleType, int hwChannelId1, void* pBufferA1, void* pBufferB1, int hwChannelId2, void* pBufferA2, void* pBufferB2, int sampleCount)
{
	IOutputChannelPairPtr outputPair = nullptr;

	switch (sampleType)
	{
	case Int32LSB:
		outputPair = new OutputInt32ChannelPair(hwChannelId1, (int*)pBufferA1, (int*)pBufferB1, hwChannelId2, (int*)pBufferA2, (int*)pBufferB2, sampleCount);
		break;
	case Int24LSB:
		outputPair = new OutputInt24ChannelPair(hwChannelId1, (byte*)pBufferA1, (byte*)pBufferB1, hwChannelId2, (byte*)pBufferA2, (byte*)pBufferB2, sampleCount);
		break;
	case Float32LSB:
		outputPair = new OutputFloat32ChannelPair(hwChannelId1, (float*)pBufferA1, (float*)pBufferB1, hwChannelId2, (float*)pBufferA2, (float*)pBufferB2, sampleCount);
		break;
	default:
		throw AudioFoundationException("Unsupported sample type.", E_UNEXPECTED);
	}

	if (outputPair == nullptr)
		throw AudioFoundationException("Not enough memory for OutputChannelPair instance.", E_OUTOFMEMORY);

	return outputPair;
}

int ObjectFactory::NextTakeId()
{
	// HACK: This could overflow after a few billions of added takes
	return ++LastTakeId;
}
