#include "pch.h"
#include "Looper.h"
#include <ISampleSource.h>
#include <FoundationObjectFactory.h>
#include <VstObjectFactory.h>
#include <AsioCoreException.h>

using namespace Audio::Asio;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Vst::Unmanaged;

Looper* Looper::Create(const ILooperConfig& config)
{
	try
	{
		if (config.OutputChannelCount == 0)
		{
			throw new AsioCoreException("Number of selected output channels cannot be zero.");
		}
		if (config.InputChannelCount == 0)
		{
			throw new AsioCoreException("Number of selected input channels cannot be zero.");
		}

		AsioCorePtr device = AsioCore::CreateInstancePtr(config.AsioDevice);

		if (device == nullptr)
		{
			throw std::runtime_error("Initialization of ASIO device failed.");
		}

		int sampleCount = config.SampleCount == 0 ? AsioCore::UsePreferredSize : config.SampleCount;

		device->CreateBuffers(config.InputChannelList, static_cast<int>(config.InputChannelCount), 
			config.OutputChannelList, static_cast<int>(config.OutputChannelCount), sampleCount, config.OutputSaturation);

		Looper* pLooper = new Looper(device);
		pLooper->AddRef();

		pLooper->Name = config.Name;
		pLooper->CreateController(config.MidiInput);
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
	m_vstHost(nullptr),
	m_delay(1000),
	m_isSessionRecording(false),
	m_recordingStatus(RecordingStatusType::Off),
	m_context(nullptr),
	m_stopCalled(false),
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
	if (iid == _uuidof(ILooper))
	{
		return dynamic_cast<ILooper*>(this);
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
	if (m_controller == nullptr)
	{
		throw std::runtime_error("Property 'Controller' must be set before running the looper.");
	}

	m_stopCalled = false;

	ControllerCode controllerCommand = ControllerCode::None;
	ITransportPtr transport = m_device->ProcessingChain->Transport;
	m_context = &transport->Context;

	// start audio device
	m_device->Start();

	// activate processing
	m_controller->IsActive = true;

	while (controllerCommand != ControllerCode::Stop)
	{
		// check for control input (MIDI)
		bool hasControl = m_controller->GetNext(m_delay, controllerCommand);

		if (m_stopCalled)
		{
			hasControl = true;
			controllerCommand = ControllerCode::Stop;
		}
		if (hasControl)
		{
			switch (controllerCommand)
			{
			case ControllerCode::Record:
				if (m_context->IsLooping)
				{
					switch(m_recordingStatus)
					{
					case RecordingStatusType::Recording:
						UnarmRecording();
						break;
					case RecordingStatusType::Off:
						ArmRecording();
						break;
					}
				}
				else 
				{
					switch(m_recordingStatus)
					{
					case RecordingStatusType::Recording:
						StopRecording();
						break;
					case RecordingStatusType::Off:
						StartRecording();
						transport->Run();
						break;
					}
				}
				break;
			case ControllerCode::Locate:
				OnLoopRestart();

				// Triggered by master loop wrap: delayed handling of 'Record' commands
				switch(m_recordingStatus)
				{
				case RecordingStatusType::Armed:
					StartRecording();
					break;
				case RecordingStatusType::Unarmed:
					StopRecording();
					break;
				}

				break;
			case ControllerCode::Run:
				// Drop current recording, stop recording, continue looping
				switch(m_recordingStatus)
				{
				case RecordingStatusType::Recording:
					DropRecording();

					if (IsLooping)
					{
						ArmRecording();
					}
					break;
				case RecordingStatusType::Armed:
					UnarmRecording();
					break;
				}
				break;

			case ControllerCode::Stop:
				// Drop current recording, stop recording, exit looping
				transport->Stop();
				if (m_recorder->IsActive)
				{
					DropRecording();
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
	m_controller->IsActive = false;
	m_device->Stop();
}

void Looper::Stop()
{
	m_stopCalled = true;
}

void Looper::ArmRecording()
{
	m_recordingStatus = RecordingStatusType::Armed;
	OnRecordingStatusChanged();
}

void Looper::UnarmRecording()
{
	if (m_recordingStatus == RecordingStatusType::Armed)
	{
		m_recordingStatus = RecordingStatusType::Off;
	}
	else
	{
		m_recordingStatus = RecordingStatusType::Unarmed;
	}
	OnRecordingStatusChanged();
}

void Looper::StartRecording()
{
	m_recorder->IsActive = true;

	if (m_isSessionRecording)
	{
		m_sessionRecorder->IsActive = true;
		OnIsSessionRecordingChanged();
	}
	m_recordingStatus = RecordingStatusType::Recording;

	OnRecordingStatusChanged();
}

void Looper::StopRecording()
{
	int switchSamplePos = m_context->SamplePosition;

	m_recordingStatus = RecordingStatusType::Off;

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
			OnIsLoopingChanged();
		}
	}
	OnRecordingStatusChanged();
}

bool Looper::DropRecording()
{
	if (m_recorder->IsActive)
	{
		m_recorder->DropRecording(false);
		m_recordingStatus = RecordingStatusType::Off;

		OnDropRecording();
		OnRecordingStatusChanged();

		return true;
	}
	return false;
}

bool Looper::AddLoop()
{
	ISampleContainerPtr take = m_recorder->CreateSampleContainer(false, 0, 0);

	if (take != nullptr)
	{
		MixParameter mix;

		ISampleSourcePtr takeSource = FoundationObjectFactory::CreateContainerSource(take);
		takeSource->IsLooping = true;
		m_joiner->AddSource(takeSource, mix);

		OnAddLoop(takeSource->Id, take->ChannelCount, takeSource->SamplePosition, take->SampleCount);
		return true;
	}
	return false;
}

MixParameter& Looper::get_LoopParameter(const GUID& id)
{
	return m_joiner->Parameter[id];
}

bool Looper::RemoveLoop(const GUID& id)
{
	return m_joiner->RemoveSource(id);
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

void Looper::OnLoopRestart()
{
	if (m_events != nullptr)
	{
		m_events->LoopRestart(*this);
	}
}

void Looper::OnIsLoopingChanged()
{
	if (m_events != nullptr)
	{
		m_events->IsLoopingChanged(*this, m_context->IsLooping);
	}
}

void Looper::OnAddLoop(const GUID& id, int channelCount, int samplePosition, int sampleCount)
{
	if (m_events != nullptr)
	{
		m_events->AddLoop(*this, id, channelCount, samplePosition, sampleCount);
	}
}

void Looper::OnDropRecording()
{
	if (m_events != nullptr)
	{
		m_events->DropRecording(*this);
	}
}

void Looper::OnIsSessionRecordingChanged()
{
	if (m_isSessionRecording && m_events != nullptr)
	{
		m_events->IsSessionRecordingChanged(*this, m_sessionRecorder->IsActive);
	}
}

void Looper::OnRecordingStatusChanged()
{
	if (m_events != nullptr)
	{
		m_events->RecordingStatusChanged(*this, m_recordingStatus);
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

RecordingStatusType Looper::get_RecordingStatus() const
{
	return m_recordingStatus;
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

IControllerPtr Looper::get_Controller()
{
	return m_controller;
}

void Looper::put_Controller(IControllerPtr& value)
{
	if (m_controller != nullptr && m_controller->IsActive)
	{
		throw std::runtime_error("Controller cannot be changed while looper is running.");
	}
	m_controller = value;
}

void Looper::CreateController(unsigned int midiInput)
{
	if (m_device == nullptr)
	{
		throw std::runtime_error("ASIO device must be created first.");
	}

	m_controller = FoundationObjectFactory::CreateMidiTransportControl(m_device->ProcessingChain->Transport, midiInput);

	if (m_controller == nullptr)
	{
		throw std::runtime_error("Initialization of Controller failed.");
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

	m_recorder = FoundationObjectFactory::CreateRecorder(processingChain->InputChannelCount, samplesPerTenSecs, samplesPerTenSecs);
	ISampleProcessorPtr recordingProcessor = nullptr;
	m_recorder->QueryInterface<ISampleProcessor>(&recordingProcessor);
	recordingProcessor->IsBypassed = true;

	m_sessionRecorder = FoundationObjectFactory::CreateRecorder(processingChain->OutputChannelPairCount * 2, samplesPerTenSecs, samplesPerTenSecs);
	ISampleProcessorPtr masterRecProcessor = nullptr;
	m_sessionRecorder->QueryInterface<ISampleProcessor>(&masterRecProcessor);
	masterRecProcessor->IsBypassed = true;

	m_joiner = FoundationObjectFactory::CreateSourceJoiner();
	ISampleProcessorPtr joiningProcessor = nullptr;
	m_joiner->QueryInterface<ISampleProcessor>(&joiningProcessor);

	int recorderId = processingChain->AddProcessor(recordingProcessor);
	int joinerId = processingChain->AddProcessor(joiningProcessor);
	processingChain->MixRecorder = masterRecProcessor;

	if (processingChain->OutputChannelPairCount > 0)
	{
		processingChain->OutputChannelPair[0]->IsActive = true;
	}
	if (processingChain->InputChannelCount > 0)
	{
		processingChain->InputChannel[0]->IsActive = true;
	}
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
