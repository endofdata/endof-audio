#include "pch.h"
#include "AudioProcessor.h"
#include <ipluginbase.h>

using namespace Audio::Vst::Unmanaged;
using namespace Steinberg::Vst;
using namespace Steinberg;

AudioProcessor::AudioProcessor(IAudioProcessorPtr& processor, IParameterChangesPtr& parameterChanges, int sampleCount, int sampleRate) :
	m_processor(processor),
	m_isActive(false),
	m_isBypassed(true),
	m_canProcess64Bit(false)
{
	IComponent* pComponent = nullptr;
	m_processor->queryInterface(IComponent_iid, reinterpret_cast<void**>(&pComponent));
	m_component = IComponentPtr(pComponent, false);

	Initialize(sampleCount, sampleRate);
}

AudioProcessor::~AudioProcessor()
{
}

AudioProcessor& AudioProcessor::operator =(const AudioProcessor& other)
{
	m_processor = other.m_processor;
	m_component = other.m_component;
	m_isActive = other.m_isActive;
	m_isBypassed = other.m_isBypassed;

	return *this;
}

IMPLEMENT_IUNKNOWN(AudioProcessor)

bool AudioProcessor::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}

	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}

	*ppvResult = nullptr;
	return false;
}

bool AudioProcessor::get_IsActive() const
{
	return m_isActive;
}

void AudioProcessor::put_IsActive(bool value)
{
	if (value != m_isActive)
	{
		m_component->setActive(value);
		m_isActive = value;
	}
}

bool AudioProcessor::get_IsBypassed()
{
	return m_isBypassed;
}

void AudioProcessor::put_IsBypassed(bool value)
{
	if (value != m_isBypassed)
	{
		m_processor->setProcessing(!value);
		m_isBypassed = value;
	}
}

void AudioProcessor::Initialize(int sampleCount, int sampleRate)
{
	m_processor->setProcessing(false);
	m_component->setActive(false);

	m_canProcess64Bit = m_processor->canProcessSampleSize(kSample64) == kResultOk;

	bool acceptBusArrangment = ConfigureBusArrangements();

	InitProcessData(sampleCount, sampleRate);

	ProcessSetup defaultSetup
	{
		kRealtime,
		m_canProcess64Bit ? kSample64 : kSample32,
		sampleCount,
		sampleRate
	};

	m_processor->setupProcessing(defaultSetup);

	IsActive = true;
}

bool AudioProcessor::ConfigureBusArrangements()
{
	int componentInputBusCount = m_component->getBusCount(kAudio, kInput);
	int componentOutputBusCount = m_component->getBusCount(kAudio, kOutput);

	if (componentInputBusCount >= 1 && componentOutputBusCount >= 1)
	{
		SpeakerArrangement inputArrangement;
		m_processor->getBusArrangement(kInput, 0, inputArrangement);
		int inputChannels = SpeakerArr::getChannelCount(inputArrangement);

		SpeakerArrangement outputArrangement;
		m_processor->getBusArrangement(kOutput, 0, outputArrangement);
		int outputChannels = SpeakerArr::getChannelCount(outputArrangement);

		tresult acceptBusArrangement = m_processor->setBusArrangements(&inputArrangement, 1, &outputArrangement, 1);

		if (acceptBusArrangement == kResultOk)
		{
			return true;
		}
	}
	return false;
}

int AudioProcessor::Process(ISampleContainerPtr& container, const ProcessingContext& context) 
{
	UpdateProcessData(container);
	m_processor->process(m_processData);

	return container->SampleCount;
}

void AudioProcessor::InitProcessData(int sampleCount, int sampleRate)
{
	int componentInputBusCount = m_component->getBusCount(kAudio, kInput);
	int componentOutputBusCount = m_component->getBusCount(kAudio, kOutput);

	m_processData.numSamples = sampleCount;
	m_inputBusBuffers.channelBuffers32 = m_pSamplePtr;
	m_outputBusBuffers.channelBuffers32 = m_pSamplePtr;

	m_processData.numInputs = std::min(componentInputBusCount, 1);
	m_processData.numOutputs = std::min(componentOutputBusCount, 1);
	m_processData.inputs = &m_inputBusBuffers;
	m_processData.outputs = &m_outputBusBuffers;

	m_processData.inputParameterChanges = m_parameterChanges;
	m_processData.outputParameterChanges = nullptr;
	m_processData.inputEvents = nullptr;
	m_processData.outputEvents = nullptr;
	m_processData.processContext = nullptr;
}

void AudioProcessor::UpdateProcessData(ISampleContainerPtr& container)
{
	int maxChannels = std::min(container->ChannelCount, static_cast<int>(_countof(m_pSamplePtr)));
	m_inputBusBuffers.numChannels = maxChannels;
	m_outputBusBuffers.numChannels = maxChannels;

	for (int c = 0; c < maxChannels; c++)
	{
		m_pSamplePtr[c] = container->Channels[c]->SamplePtr;
	}
}
