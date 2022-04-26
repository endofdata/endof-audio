#include "pch.h"
#include "ObjectFactory.h"
#include "ChannelLink.h"
#include "MeterChannel.h"
#include "SampleBuffer.h"
#include "SampleContainer.h"
#include "SampleJoiner.h"
#include "SampleSharer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

// TODO: Constructors must AddRef for received interfaces
// 
// static
IChannelLink* ObjectFactory::CreateChannelLink()
{
	ChannelLink* instance = new ChannelLink();
	IChannelLink* result;
	if (FAILED(instance->QueryInterface(__uuidof(IChannelLink), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}

// static
IChannelLink* ObjectFactory::CreateChannelLink(ISampleContainer* pInput, ISampleReceiver* pOutput, float volume, float pan)
{
	ChannelLink* instance = new ChannelLink(pInput, pOutput, volume, pan);
	IChannelLink* result;
	if (FAILED(instance->QueryInterface(__uuidof(IChannelLink), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}

// static
IMeterChannel* ObjectFactory::CreateMeterChannel(int sampleRate)
{
	MeterChannel* instance = new MeterChannel(sampleRate);
	IMeterChannel* result;
	if (FAILED(instance->QueryInterface(__uuidof(IMeterChannel), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}

// static
ISampleBuffer* ObjectFactory::CreateSampleBuffer(int sampleCount)
{
	SampleBuffer* instance = new SampleBuffer(sampleCount);
	ISampleBuffer* result;
	if (FAILED(instance->QueryInterface(__uuidof(ISampleBuffer), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}

// static
ISampleJoiner* ObjectFactory::CreateSampleJoiner(int sampleCount)
{
	SampleJoiner* instance = new SampleJoiner(sampleCount);
	ISampleJoiner* result;
	if (FAILED(instance->QueryInterface(__uuidof(ISampleJoiner), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}

// static
ISampleContainer* ObjectFactory::CreateSampleContainer(int sampleCount)
{
	SampleContainer* instance = new SampleContainer(sampleCount);
	ISampleContainer* result;
	if (FAILED(instance->QueryInterface(__uuidof(ISampleContainer), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}

// static
ISampleSharer* ObjectFactory::CreateSampleSharer()
{
	SampleSharer* instance = new SampleSharer();
	ISampleSharer* result;
	if (FAILED(instance->QueryInterface(__uuidof(ISampleSharer), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}
