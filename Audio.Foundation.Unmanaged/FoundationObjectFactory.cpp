#include "pch.h"
#include "FoundationObjectFactory.h"
#include "MeterChannel.h"
#include "SampleBuffer.h"
#include "SampleContainer.h"
#include "SourceJoiner.h"
#include "SampleSharer.h"
#include "FileWriter.h"
#include "FileReader.h"
#include "ContainerReader.h"
#include "ContainerWriter.h"
#include "VectorWriter.h"
#include "Take.h"
#include "TakeSequence.h"
#include "InputInt32Channel.h"
#include "InputInt24Channel.h"
#include "InputFloat32Channel.h"
#include "OutputInt32ChannelPair.h"
#include "SatOutputInt32ChannelPair.h"
#include "OutputInt24ChannelPair.h"
#include "SatOutputInt24ChannelPair.h"
#include "OutputFloat32ChannelPair.h"
#include "SatOutputFloat32ChannelPair.h"
#include "AudioFoundationException.h"
#include "ProcessingChain.h"
#include "SourceProcessor.h"
#include "HostClock.h"
#include "MidiInput.h"
#include "MidiEvents.h"
#include "Transport.h"
#include "ControllerEvents.h"
#include "MidiTransportControl.h"
#include "GainProcessor.h"
#include "Oscillator.h"
#include <stdexcept>

int FoundationObjectFactory::LastTakeId = 0;

using namespace Audio::Foundation::Unmanaged;


ISampleBufferPtr FoundationObjectFactory::CreateSampleBuffer(int sampleCount)
{
	return new SampleBuffer(sampleCount);
}

ISampleContainerPtr FoundationObjectFactory::CreateSampleContainer(int sampleCount, int channelCount)
{
	return new SampleContainer(sampleCount, channelCount);
}

ISampleSharerPtr FoundationObjectFactory::CreateSampleSharer()
{
	return new SampleSharer();
}

ISourceJoinerPtr FoundationObjectFactory::CreateSourceJoiner()
{
	return new SourceJoiner();
}

IMeterChannelPtr FoundationObjectFactory::CreateMeterChannel(int sampleRate, int channelCount)
{
	return new MeterChannel(sampleRate, channelCount);
}

ISampleProcessorPtr FoundationObjectFactory::CreateToFileProcessor(const std::wstring& filename)
{
	return new FileWriter(filename);
}

ISampleProcessorPtr FoundationObjectFactory::CreateToStreamProcessor(std::ostream& output)
{
	return new StreamWriter(output);
}

ISampleProcessorPtr FoundationObjectFactory::CreateToContainerProcessor(ISampleContainerPtr& target)
{
	return new ContainerWriter(target);
}

ISpatialPtr FoundationObjectFactory::CreateSpatial(double level, double pan)
{
	return new GainProcessor(level, pan);
}

IRecorderPtr FoundationObjectFactory::CreateRecorder(int channelCount, int initialSize, int growth)
{
	return new VectorWriter(channelCount, initialSize, growth);
}

ISampleSourcePtr FoundationObjectFactory::CreateRawFileSource(const std::wstring& filename, int channelCount)
{
	return FileReader::Create(filename, channelCount);
}

ISampleSourcePtr FoundationObjectFactory::CreateContainerSource(ISampleContainerPtr& source)
{
	return new ContainerReader(source);
}

ISampleProcessorPtr FoundationObjectFactory::CreateFromSourceProcessor(ISampleSourcePtr& source)
{
	return new SourceProcessor(source);
}

ITakePtr FoundationObjectFactory::CreateTake(ISampleContainerPtr& container, AudioTime position, IHostClockPtr& hostClock)
{
	AudioTime length = AudioTime::FromSeconds(container->SampleCount / hostClock->SampleRate);

	return CreateTake(container, position, length);
}

ITakePtr FoundationObjectFactory::CreateTake(ISampleContainerPtr& container, AudioTime position, AudioTime length)
{
	return new Take(NextTakeId(), container, position, length);
}

IHostClockPtr FoundationObjectFactory::CreateHostClock(double sampleRate)
{	
	if (sampleRate == 0.0)
	{
		sampleRate = 48000.0;
	}

	return new HostClock(std::max(8000.0, sampleRate));
}

ITransportPtr FoundationObjectFactory::CreateTransport(IHostClockPtr& hostClock, int sampleCount)
{
	IControllerEventsPtr events = new ControllerEvents();
	return new Transport(hostClock, events, sampleCount);
}

IControllerPtr FoundationObjectFactory::CreateMidiTransportControl(ITransportPtr& transport, int midiDevId)
{
	if (midiDevId >= 0)
	{
		IMidiInputPtr midiInput = FoundationObjectFactory::CreateMidiInput();

		if (midiInput->Open(midiDevId))
		{
			return IControllerPtr(new MidiTransportControl(midiInput, transport));
		}
	}
	return nullptr;
}

ITakeSequencePtr FoundationObjectFactory::CreateTakeSequence(ITransportPtr& transport)
{
	return new TakeSequence(transport);
}

IInputChannelPtr FoundationObjectFactory::CreateInputChannel(int sampleType, int hwChannelId, void* pHwBufferA, void* pHwBufferB, int sampleCount)
{
	switch (sampleType)
	{
	case Int32LSB:
		return new InputInt32Channel(hwChannelId, reinterpret_cast<int*>(pHwBufferA), reinterpret_cast<int*>(pHwBufferB), sampleCount);
	case Int24LSB:
		return new InputInt24Channel(hwChannelId, reinterpret_cast<byte*>(pHwBufferA), reinterpret_cast<byte*>(pHwBufferB), sampleCount);
	case Float32LSB:
		return new InputFloat32Channel(hwChannelId, reinterpret_cast<float*>(pHwBufferA), reinterpret_cast<float*>(pHwBufferB), sampleCount);
	default:
		throw AudioFoundationException("Unsupported sample type.", E_UNEXPECTED);
	}
}

IOutputChannelPairPtr FoundationObjectFactory::CreateOutputChannelPair(int sampleType, int hwChannelId1, void* pBufferA1, void* pBufferB1, 
	int hwChannelId2, void* pBufferA2, void* pBufferB2, int sampleCount, float saturation)
{
	IOutputChannelPairPtr outputPair = nullptr;

	switch (sampleType)
	{
	case Int32LSB:
		outputPair = saturation > 0.0f? 
			reinterpret_cast<IOutputChannelPair*>(new SatOutputInt32ChannelPair(
				hwChannelId1, reinterpret_cast<int*>(pBufferA1), reinterpret_cast<int*>(pBufferB1), 
				hwChannelId2, reinterpret_cast<int*>(pBufferA2), reinterpret_cast<int*>(pBufferB2), 
				sampleCount, saturation)) :
			new OutputInt32ChannelPair(
				hwChannelId1, reinterpret_cast<int*>(pBufferA1), reinterpret_cast<int*>(pBufferB1), 
				hwChannelId2, reinterpret_cast<int*>(pBufferA2), reinterpret_cast<int*>(pBufferB2), 
				sampleCount);
		break;
	case Int24LSB:
		outputPair = saturation > 0.0f ?
			reinterpret_cast<IOutputChannelPair*>(new SatOutputInt24ChannelPair(
				hwChannelId1, reinterpret_cast<byte*>(pBufferA1), reinterpret_cast<byte*>(pBufferB1), 
				hwChannelId2, reinterpret_cast<byte*>(pBufferA2), reinterpret_cast<byte*>(pBufferB2), 
				sampleCount, saturation)) :
			new OutputInt24ChannelPair(
				hwChannelId1, reinterpret_cast<byte*>(pBufferA1), reinterpret_cast<byte*>(pBufferB1), 
				hwChannelId2, reinterpret_cast<byte*>(pBufferA2), reinterpret_cast<byte*>(pBufferB2), 
				sampleCount);
		break;
	case Float32LSB:
		outputPair = saturation > 0.0f ?
			reinterpret_cast<IOutputChannelPair*>(new SatOutputFloat32ChannelPair(
				hwChannelId1, reinterpret_cast<float*>(pBufferA1), reinterpret_cast<float*>(pBufferB1), 
				hwChannelId2, reinterpret_cast<float*>(pBufferA2), reinterpret_cast<float*>(pBufferB2), 
				sampleCount, saturation)) :
			new OutputFloat32ChannelPair(
				hwChannelId1, reinterpret_cast<float*>(pBufferA1), reinterpret_cast<float*>(pBufferB1), 
				hwChannelId2, reinterpret_cast<float*>(pBufferA2), reinterpret_cast<float*>(pBufferB2), 
				sampleCount);
		break;
	default:
		throw AudioFoundationException("Unsupported sample type.", E_UNEXPECTED);
	}

	if (outputPair == nullptr)
		throw AudioFoundationException("Not enough memory for OutputChannelPair instance.", E_OUTOFMEMORY);

	return outputPair;
}

IProcessingChainPtr FoundationObjectFactory::CreateProcessingChain(ITransportPtr& transport, ISampleContainerPtr& container)
{
	return new ProcessingChain(transport, container);
}

IOscillatorPtr FoundationObjectFactory::CreateTestOscillator(double sampleRate, double frequency, double amplitude)
{
	Oscillator* oscillator = new Oscillator(sampleRate);
	oscillator->Frequency = frequency;
	oscillator->Amplitude = amplitude;

	return oscillator;
}

int FoundationObjectFactory::SelectMidiInputDevice(MidiInCapsHandler handler, void* callbackParam)
{
	return MidiInput::ListDevices(handler, callbackParam);
}

IMidiInputPtr FoundationObjectFactory::CreateMidiInput()
{
	IMidiEventsPtr events = new MidiEvents();
	return new MidiInput(events);
}

int FoundationObjectFactory::NextTakeId()
{
	// HACK: This could overflow after a few billions of added takes
	return ++LastTakeId;
}
