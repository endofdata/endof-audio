#include "pch.h"
#include "Looper.h"
#include <ISampleSource.h>
#include <ObjectFactory.h>
#include <VstObjectFactory.h>
#include <AsioCoreException.h>

using namespace Audio::Asio;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Vst::Unmanaged;

Looper* Looper::Create(const LooperConfig& config, const wchar_t* pwcszName)
{
	try
	{
		AsioCorePtr device = AsioCore::CreateInstancePtr(config.AsioDevice);

		if (device == nullptr)
		{
			throw std::runtime_error("Initialization of ASIO device failed.");
		}

		device->CreateBuffers(config.InputChannelList, config.InputChannelCount, config.OutputChannelList, static_cast<int>(config.OutputChannelCount), config.SampleCount, config.OutputSaturation);

		Looper* pLooper = new Looper(device);
		pLooper->AddRef();

		pLooper->Name = pwcszName;
		pLooper->CreateTransportControl(config.MidiInput);
		pLooper->CreateVstHost();
		pLooper->CreateProcessingChain();

		return pLooper;
	}
	catch (const AsioCoreException& acx)
	{
		std::ostringstream builder;
		builder << "Failed to create device. " << acx.Message << " Error code: 0x" << std::hex << acx.Error << std::dec;

		throw std::runtime_error(builder.str());
	}
}

Looper::Looper(AsioCorePtr& device) :
	m_device(device),
	m_delay(1000),
	m_isSessionRecording(false),
	m_context(nullptr),
	m_refCount(0)
{
}

Looper::~Looper()
{
}

IMPLEMENT_IUNKNOWN(Looper)

void* Looper::GetInterface(const IID& iid)
{
	if (iid == _uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	return nullptr;
}

bool Looper::SelectInput(int input, bool isSelected)
{
	IProcessingChainPtr processingChain = m_device->ProcessingChain;

	if (input < 0 || input >= processingChain->InputChannelCount)
	{
		throw std::invalid_argument("Input index is out of range.");
	}
	processingChain->InputChannel[input]->IsActive = isSelected;

	return true;
}

bool Looper::SelectOutputPair(int outputPair[2], bool isSelected)
{
	IProcessingChainPtr processingChain = m_device->ProcessingChain;

	// TODO: Output channel pairing is still inconsistent
	processingChain->OutputChannelPair[0]->IsActive = isSelected;

	return true;
}

const wchar_t* Looper::AddVstPlugin(const wchar_t* pwcszVstLibrary)
{
	const wchar_t* pwcszPluginId = m_vstHost->AddLibrary(pwcszVstLibrary);

	if (pwcszPluginId == nullptr)
	{
		throw std::runtime_error("Cannot load VST library.");
	}
	return pwcszPluginId;
}

int Looper::InsertFx(const wchar_t* pwcszPluginId)
{
	if (pwcszPluginId == nullptr)
	{
		throw std::invalid_argument("Parameter 'pwcszRawId' cannot be null.");
	}

	ISampleProcessorPtr vstProcessor = m_vstHost->CreateSampleProcessor(pwcszPluginId);

	if (vstProcessor == nullptr)
	{
		throw std::runtime_error("Cannot create sample processor from VST plugin.");
	}
	return InsertFx(vstProcessor);
}

int Looper::InsertFx(ISampleProcessorPtr effect)
{
	IProcessingChainPtr processingChain = m_device->ProcessingChain;
	return processingChain->AddProcessor(effect);
}

bool Looper::RemoveFx(int id)
{
	IProcessingChainPtr processingChain = m_device->ProcessingChain;
	return processingChain->RemoveProcessor(id);
}

void Looper::Run()
{
	if (m_transportControl == nullptr)
	{
		throw std::runtime_error("Property 'TransportControl' must be set before running the looper.");
	}

	bool isRecordPressed = false;
	TransportCode previousStatus = TransportCode::None;
	TransportCode transportStatus = TransportCode::None;
	ITransportPtr transport = m_device->ProcessingChain->Transport;
	m_context = &transport->Context;

	// start audio device
	m_device->Start();

	// activate processing
	m_transportControl->IsActive = true;

	while (transportStatus != TransportCode::Stop)
	{
		// check for control input (MIDI)
		bool hasControl = m_transportControl->GetNext(m_delay, transportStatus);

		if (hasControl)
		{
			if (previousStatus != transportStatus)
			{
				OnTransportStatusChanged(previousStatus, transportStatus);
				previousStatus = transportStatus;
			}

			switch (transportStatus)
			{
			case TransportCode::Record:
				if (!m_context->IsLooping)
				{
					ToggleRecording(transport);
				}
				else
				{
					isRecordPressed = !isRecordPressed;
				}
				break;
			case TransportCode::Locate:
				OnLoopRestart();

				// Triggered by master loop wrap: delayed handling of 'Record' commands
				if (isRecordPressed)
				{
					isRecordPressed = false;
					ToggleRecording(transport);
				}
				break;
			case TransportCode::Start:
			{
				// Drop current recording, stop recording, continue looping
				if (m_recorder->IsActive)
				{
					DropRecording(true);
				}
			}
			break;

			case TransportCode::Stop:
				// Drop current recording, stop recording, exit looping
				transport->Stop();
				if (m_recorder->IsActive)
				{
					DropRecording(false);
				}
				break;
			default:
				break;
			}
		}
		OnHeartbeat(transport);
	}
	m_context = nullptr;
	m_device->ProcessingChain->InitShutDown();
	m_transportControl->IsActive = false;
	m_device->Stop();
}



bool Looper::ToggleRecording(ITransportPtr& transport)
{
	if (!m_recorder->IsActive)
	{
		m_recorder->IsActive = true;
		m_sessionRecorder->IsActive = m_isSessionRecording;

		if (!m_context->IsLooping)
		{
			transport->Start();
		}
	}
	else
	{
		int switchSamplePos = m_context->SamplePosition;
	
		m_sessionRecorder->IsActive = false;

		if (AddLoop())
		{
			if (m_context->IsLooping)
			{
				// TODO: Add threshold to prevent unintentional resize? Does resize work at all?
				if (switchSamplePos > m_context->LoopEndSample)
				{
					m_context->LoopEndSample = switchSamplePos;
				}
			}
			else
			{
				m_context->LoopEndSample = switchSamplePos;
				m_context->IsLooping = true;
			}
		}
	}
	OnSessionRecordingChanged();
	OnLoopRecordingChanged();

	return m_recorder->IsActive;
}

bool Looper::AddLoop()
{
	ISampleContainerPtr take = m_recorder->CreateSampleContainer(false, 0, 0);

	if (take != nullptr)
	{
		MixParameter mix;

		ISampleSourcePtr takeSource = ObjectFactory::CreateContainerSource(take);
		takeSource->IsLooping = true;
		m_joiner->AddSource(takeSource, mix);

		OnAddLoop();
		return true;
	}
	return false;
}

bool Looper::DropRecording(bool continueRecording)
{
	if (m_recorder->IsActive)
	{
		m_recorder->DropRecording(continueRecording);
		OnDropRecording(continueRecording);

		return true;
	}
	return false;
}

void Looper::SaveSession(const wchar_t* pwcszFilenameBase)
{
	ISampleContainerPtr container = m_sessionRecorder->CreateSampleContainer(false, 0, 0);

	// TODO: Write WAV format
	if (container != nullptr)
	{
		for (int c = 0; c < container->ChannelCount; c++)
		{
			std::wostringstream builder;
			builder << pwcszFilenameBase << c << L".dat" << std::ends;

			auto filename = builder.str();

			std::ofstream out(filename, std::ios_base::out + std::ios_base::binary + std::ios_base::trunc);
			auto data = reinterpret_cast<const char*>(container->Channels[c]->SamplePtr);

			out.write(data, container->SampleCount * sizeof(Sample));
			out.flush();
		}
	}
}

void Looper::OnHeartbeat(ITransportPtr& transport)
{
	if (m_events != nullptr)
	{
		m_events->Heartbeat(*this, transport);
	}
}

void Looper::OnTransportStatusChanged(TransportCode previous, TransportCode current)
{
	if (m_events != nullptr)
	{
		m_events->TransportStatusChanged(*this, previous, current);
	}
}

void Looper::OnLoopRestart()
{
	if (m_events != nullptr)
	{
		m_events->LoopRestart(*this);
	}
}

void Looper::OnLoopRecordingChanged()
{
	if (m_events != nullptr)
	{
		m_events->LoopRecordingChanged(*this, m_recorder->IsActive);
	}
}

void Looper::OnAddLoop()
{
	if (m_events != nullptr)
	{
		m_events->AddLoop(*this);
	}
}

void Looper::OnDropRecording(bool continueRecording)
{
	if (m_events != nullptr)
	{
		m_events->DropRecording(*this, continueRecording);
	}
}

void Looper::OnSessionRecordingChanged()
{
	if (m_isSessionRecording && m_events != nullptr)
	{
		m_events->SessionRecordingChanged(*this, m_sessionRecorder->IsActive);
	}
}

bool Looper::get_IsLooping() const
{
	return m_context != nullptr && m_context->IsLooping;
}

int Looper::get_LoopCount() const
{
	return m_joiner->SourceCount;
}

bool Looper::get_IsLoopRecording() const
{
	return m_recorder->IsActive;
}

bool Looper::get_IsSessionRecording() const
{
	return m_isSessionRecording;
}

void Looper::put_IsSessionRecording(bool value)
{
	m_isSessionRecording = value;
}

const wchar_t* Looper::get_Name() const
{
	return m_name.c_str();
}

void Looper::put_Name(const wchar_t* value)
{
	if (value == nullptr)
	{
		m_name.clear();
	}
	else
	{
		m_name = value;
	}
}

ILooperEventsPtr Looper::get_LooperEvents()
{
	return m_events;
}

void Looper::put_LooperEvents(ILooperEventsPtr& value)
{
	m_events = value;
}

void Looper::CreateTransportControl(unsigned int midiInput)
{
	if (m_device == nullptr)
	{
		throw std::runtime_error("ASIO device must be created first.");
	}

	m_transportControl = ObjectFactory::CreateMidiTransportControl(m_device->ProcessingChain->Transport, midiInput);

	if (m_transportControl == nullptr)
	{
		throw std::runtime_error("Initialization of TransportControl failed.");
	}
}

void Looper::CreateProcessingChain()
{
	if (m_device == nullptr)
	{
		throw std::runtime_error("ASIO device must be created first.");
	}

	IProcessingChainPtr processingChain = m_device->ProcessingChain;
	int samplesPerTenSecs = static_cast<int>(m_device->SampleRate * 60.0);

	m_recorder = ObjectFactory::CreateRecorder(processingChain->InputChannelCount, samplesPerTenSecs, samplesPerTenSecs);
	ISampleProcessorPtr recordingProcessor = nullptr;
	m_recorder->QueryInterface<ISampleProcessor>(&recordingProcessor);
	recordingProcessor->IsBypassed = true;

	m_sessionRecorder = ObjectFactory::CreateRecorder(processingChain->OutputChannelPairCount * 2, samplesPerTenSecs, samplesPerTenSecs);
	ISampleProcessorPtr masterRecProcessor = nullptr;
	m_sessionRecorder->QueryInterface<ISampleProcessor>(&masterRecProcessor);
	masterRecProcessor->IsBypassed = true;

	m_joiner = ObjectFactory::CreateSourceJoiner();
	ISampleProcessorPtr joiningProcessor = nullptr;
	m_joiner->QueryInterface<ISampleProcessor>(&joiningProcessor);

	int recorderId = processingChain->AddProcessor(recordingProcessor);
	int joinerId = processingChain->AddProcessor(joiningProcessor);
	processingChain->MixRecorder = masterRecProcessor;

	processingChain->OutputChannelPair[0]->IsActive = true;
	processingChain->InputChannel[0]->IsActive = true;
}

void Looper::CreateVstHost()
{
	if (m_device == nullptr)
	{
		throw std::runtime_error("ASIO device must be created first.");
	}

	// create processor chain input -> vst -> recorder -> output
	m_vstHost = VstObjectFactory::CreateVstHost(Name, m_device->SampleCount, m_device->SampleRate);

	if (m_vstHost == nullptr)
	{
		throw std::runtime_error("Initialization of VST host failed.");
	}
}
