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
	LooperEventLog eventLog;

	try
	{
		eventLog.Status("creating looper instance");

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

		pLooper->Name = config.Name;

		if (config.ControlResolution > 0)
		{
			pLooper->m_delay = config.ControlResolution;
		}

		if (config.ControllerFactory != nullptr)
		{
			// TODO: Use InterfacePtr only where required!!
			IControllerPtr customController(config.ControllerFactory(device->ProcessingChain->Transport));
			pLooper->Controller = customController;
		}
		else if (config.MidiInput != static_cast<unsigned int>(-1))
		{
			pLooper->CreateController(config.MidiInput);
		}

		pLooper->CreateVstHost();
		pLooper->CreateProcessingChain();

		eventLog.Status("looper instance created.");

		return pLooper;
	}
	catch (const AsioCoreException& acx)
	{
		std::ostringstream builder;
		builder << "Failed to create device. " << acx.Message << " Error code: 0x" << std::hex << acx.Error << std::dec;

		std::string message = builder.str();

		eventLog.Status(message.c_str());

		throw std::runtime_error(message);
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
	m_controlThread(nullptr),
	m_controlThreadId(0),
	m_refCount(0)
{
	m_eventLog.Status("new looper instance");
}

Looper::~Looper()
{
	m_eventLog.Status("drop looper instance");
	Stop(INFINITE);
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

bool Looper::SelectInput(int inputIdx, bool isSelected)
{
	IProcessingChainPtr processingChain = m_device->ProcessingChain;

	if (inputIdx < 0 || inputIdx >= processingChain->InputChannelCount)
	{
		throw std::invalid_argument("Input index is out of range.");
	}
	IInputChannelPtr channel = processingChain->InputChannel[inputIdx];

	channel->IsActive = isSelected;

	m_eventLog.ActivateInputChannel(channel->Id, channel->IsActive);

	return true;
}

bool Looper::SelectOutputPair(int outputPairIdx, bool isSelected)
{
	IProcessingChainPtr processingChain = m_device->ProcessingChain;

	IOutputChannelPairPtr channelPair = processingChain->OutputChannelPair[outputPairIdx];
		
	channelPair->IsActive = isSelected;

	// TODO: Expose channel IDs for left and right channel
	int encodedIds = channelPair->Id;
	int leftChannel = encodedIds >> 16;
	int rightChannel = encodedIds & 0x00FF;

	m_eventLog.ActivateOutputChannelPair(leftChannel, rightChannel, channelPair->IsActive);

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

	OnStarting();

	m_eventLog.Status("running");

	ControllerCode controllerCommand = ControllerCode::None;
	ITransportPtr transport = m_device->ProcessingChain->Transport;
	m_context = &transport->Context;

	// start audio device
	m_device->Start();

	// activate controller
	m_controller->IsActive = true;

	while (controllerCommand != ControllerCode::Stop)
	{
		// check for control input (MIDI)
		bool hasControl = m_controller->GetNext(m_delay, controllerCommand);

		if (m_stopCalled)
		{
			m_stopCalled = false;
			hasControl = true;
			controllerCommand = ControllerCode::Stop;
		}
		if (hasControl)
		{
			m_eventLog.ControlCode(controllerCommand);

			switch (controllerCommand)
			{
			case ControllerCode::Record:
				// Toggle recording, for subsequent loops sync'ed with next 'Locate' command
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
				Wrap();

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
			case ControllerCode::Cancel:
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
			case ControllerCode::Pause:
				// Stop playback temporarily
				PausePlayback(transport);
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
	m_controller->IsActive = false;
	m_device->Stop();

	OnStopping();

	m_eventLog.Status("stopped");
}

DWORD Looper::ControlThreadEntry(LPVOID param)
{
	Looper* looper = static_cast<Looper*>(param);

	try
	{
		looper->Run();
		looper->ControlThreadExit();
	}
	catch (const std::exception&)
	{
		looper->ControlThreadExit();
	}

	return 0;
}

void Looper::ControlThreadExit()
{
	HANDLE controlThread = static_cast<HANDLE>(InterlockedExchangePointer(&m_controlThread, nullptr));

	if (controlThread != nullptr)
	{
		CloseHandle(controlThread);
		m_controlThreadId = 0;
		Release();
	}
}

void Looper::Start()
{
	if (IsRunning)
	{
		throw std::runtime_error("Looper is already running.");
	}

	m_eventLog.Status("starting");

	// One reference to be dropped by control thread
	AddRef();
	m_controlThread = CreateThread(NULL, 0, Looper::ControlThreadEntry, this, 0, &m_controlThreadId);

	if (!IsRunning)
	{
		Release();
		throw std::runtime_error("Failed to create looper control thread.");
	}
	do
	{
		if (m_context != nullptr)
		{
			break;
		}
	} while (m_controlThread != nullptr && WAIT_TIMEOUT == WaitForSingleObject(m_controlThread, m_delay));
}

bool Looper::Stop(DWORD waitTimeout)
{
	if (IsRunning)
	{
		m_eventLog.Status("stopping");

		m_stopCalled = true;
		return Wait(waitTimeout);
	}
	return true;
}

bool Looper::Wait(DWORD waitTimeout)
{
	return m_controlThread == nullptr || (WAIT_OBJECT_0 == WaitForSingleObject(m_controlThread, waitTimeout));
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
	int loopLength = m_context->IsLooping ?
		m_context->LoopEndSample :
		m_context->SamplePosition;

	m_recordingStatus = RecordingStatusType::Off;

	if (AddLoop(loopLength))
	{
		if (!m_context->IsLooping)
		{
			m_context->LoopEndSample = loopLength;
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

bool Looper::PausePlayback(ITransportPtr& transport)
{
	bool newValue = !transport->IsPaused;
	transport->IsPaused = newValue;
	OnIsPaused(newValue);

	return newValue;
}

bool Looper::AddLoop(int maxSamples)
{
	ISampleContainerPtr take = m_recorder->CreateSampleContainer(false, maxSamples, 0, 0);

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

void Looper::Wrap()
{
	for (int i = 0; i < m_joiner->SourceCount; i++)
	{
		ISampleSourcePtr source = m_joiner->GetSourceByIndex(i);
		if (source != nullptr)
		{
			source->SamplePosition = 0;
		}
	}
	OnLoopRestart();
}

bool Looper::RemoveLoop(const GUID& id)
{
	if (m_joiner->RemoveSource(id))
	{
		if (m_joiner->SourceCount == 0)
		{
			m_context->IsLooping = false;
			m_context->LoopEndSample = 0;
			m_context->LoopStartSample = 0;
		}
		OnRemoveLoop(id);
		return true;
	}
	return false;
}

void Looper::SaveSession(const wchar_t* pwcszFilenameBase)
{
	ISampleContainerPtr container = m_sessionRecorder->CreateSampleContainer(false, 0, 0, 0);

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

void Looper::OnStarting()
{
	if (m_events != nullptr)
	{
		m_events->Starting(*this);
	}
}

void Looper::OnStopping()
{
	if (m_events != nullptr)
	{
		m_events->Stopping(*this);
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

void Looper::OnRemoveLoop(const GUID& id)
{
	if (m_events != nullptr)
	{
		m_events->RemoveLoop(*this, id);
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
	m_eventLog.RecordingStatus(m_recordingStatus);

	if (m_events != nullptr)
	{
		m_events->RecordingStatusChanged(*this, m_recordingStatus);
	}
}

void Looper::OnIsPaused(bool isPaused)
{
	m_eventLog.Pause(isPaused);

	if (m_events != nullptr)
	{
		m_events->IsPausedChanged(*this, isPaused);
	}
}

bool Looper::get_IsRunning() const
{
	return m_controlThread != nullptr;
}

bool Looper::get_IsLooping() const
{
	return m_context != nullptr && m_context->IsLooping;
}

int Looper::get_LoopCount() const
{
	return m_joiner->SourceCount;
}

AudioTime Looper::get_LoopLength() const
{
	ITransportPtr& transport = m_device->ProcessingChain->Transport;
	return transport->LoopEndTime - transport->LoopStartTime;
}

MixParameter& Looper::get_LoopParameter(const GUID& id)
{
	return m_joiner->Parameter[id];
}

AudioTime Looper::get_TransportPosition() const
{
	return m_device->ProcessingChain->Transport->TimePosition;
}

void Looper::put_TransportPosition(Audio::Foundation::Unmanaged::AudioTime value)
{
	m_device->ProcessingChain->Transport->TimePosition = value;
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

bool Looper::get_IsPaused() const
{
	return m_device->ProcessingChain->Transport->IsPaused;
}

void Looper::put_IsPaused(bool value)
{
	m_device->ProcessingChain->Transport->IsPaused = value;
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
