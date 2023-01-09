#include "pch.h"
#include "ObjectFactory.h"
#include "MeterChannel.h"
#include "SampleBuffer.h"
#include "SampleContainer.h"
#include "SampleJoiner.h"
#include "SampleSharer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

// TODO: Constructors must AddRef for received interfaces

// static
IMeterChannel* ObjectFactory::CreateMeterChannel(int sampleRate, int channelCount)
{
	MeterChannel* instance = new MeterChannel(sampleRate, channelCount);
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
ISampleJoiner* ObjectFactory::CreateSampleJoiner(int sampleCount, int channelCount)
{
	SampleJoiner* instance = new SampleJoiner(sampleCount, channelCount);
	ISampleJoiner* result;
	if (FAILED(instance->QueryInterface(__uuidof(ISampleJoiner), (void**)&result)))
	{
		throw new std::runtime_error("QueryInterface failed");
	}
	return result;
}

// static
ISampleContainer* ObjectFactory::CreateSampleContainer(int sampleCount, int channelCount)
{
	SampleContainer* instance = new SampleContainer(sampleCount, channelCount);
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
