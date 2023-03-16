#include "pch.h"
#include "ObjectFactory.h"
#include "MeterChannel.h"
#include "SampleBuffer.h"
#include "SampleContainer.h"
#include "SampleJoiner.h"
#include "SampleSharer.h"
#include "FileWriter.h"
#include "ContainerReader.h"
#include "ContainerWriter.h"
#include "VectorWriter.h"
#include "FileSource.h"
#include "Take.h"
#include "TakeSequence.h"
#include "InputInt32Channel.h"
#include "InputInt24Channel.h"
#include "InputFloat32Channel.h"
#include "OutputInt32ChannelPair.h"
#include "OutputInt24ChannelPair.h"
#include "OutputFloat32ChannelPair.h"
#include "AudioFoundationException.h"
#include "HostClock.h"

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

ISampleProcessorPtr ObjectFactory::CreateToFileProcessor(const std::string& filename)
{
	return new FileWriter(filename);
}

ISampleProcessorPtr ObjectFactory::CreateToStreamProcessor(std::ostream& output)
{
	return new StreamWriter(output);
}

ISampleProcessorPtr ObjectFactory::CreateToContainerProcessor(ISampleContainerPtr target)
{
	return new ContainerWriter(target);
}

ISampleProcessorPtr ObjectFactory::CreateToContainerProcessor(int channelCount, int initialSize, int growth)
{
	return new VectorWriter(channelCount, initialSize, growth);
}

ISampleSourcePtr ObjectFactory::CreateFileSource(int sampleCount, int channelCount, const std::string& filename)
{
	return new FileSource(filename, CreateSampleContainer(sampleCount, channelCount));
}

ISampleSourcePtr ObjectFactory::CreateContainerSource(ISampleContainerPtr source, int sampleCount)
{
	return new ContainerReader(source, sampleCount);
}

ITakePtr ObjectFactory::CreateTake(ISampleContainerPtr container, AudioTime position, IHostClockPtr hostClock)
{
	AudioTime length = AudioTime::FromSeconds(container->SampleCount / hostClock->SampleRate);

	return CreateTake(container, position, length);
}

ITakePtr ObjectFactory::CreateTake(ISampleContainerPtr container, AudioTime position, AudioTime length)
{
	return new Take(NextTakeId(), container, position, length);
}

IHostClockPtr ObjectFactory::CreateHostClock(int sampleRate)
{	
	// If no samplerate is specified, use 48,000. Otherwise check lower limit of 8,000
	sampleRate = sampleRate == 0 ? 48000 : sampleRate < 8000 ? 8000 : sampleRate;

	return new HostClock(std::max(8000, sampleRate));
}

ITakeSequencePtr ObjectFactory::CreateTakeSequence(IHostClockPtr hostClock, ISampleContainerPtr targetContainer)
{
	return new TakeSequence(hostClock, targetContainer);
}

IInputChannelPtr ObjectFactory::CreateInputChannel(int sampleType, int hwChannelId, void* pHwBufferA, void* pHwBufferB, int sampleCount)
{
	switch (sampleType)
	{
	case Int32LSB:
		return new InputInt32Channel(hwChannelId, (int*)pHwBufferA, (int*)pHwBufferB, sampleCount);
	case Int24LSB:
		return new InputInt24Channel(hwChannelId, (byte*)pHwBufferA, (byte*)pHwBufferB, sampleCount);
	case Float32LSB:
		return new InputFloat32Channel(hwChannelId, (float*)pHwBufferA, (float*)pHwBufferB, sampleCount);
	default:
		throw AudioFoundationException("Unsupported sample type.", E_UNEXPECTED);
	}
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
