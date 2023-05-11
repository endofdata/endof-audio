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

		device->CreateBuffers(config.InputChannelList, config.InputChannelCount, config.OutputChannelList, config.OutputChannelCount, config.SampleCount, config.OutputSaturation);

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
		std::wcerr << L"Failed to create device. " << acx.Message << L" Error code: 0x" << std::hex << acx.Error << std::dec << std::endl;
	}
}

Looper::Looper(AsioCorePtr& device) :
	m_device(device),
	m_delay(1000),
	m_isRecording(false),
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
	TransportCode transportStatus = TransportCode::None;
	ITransportPtr transport = m_device->ProcessingChain->Transport;
	ProcessingContext& context = transport->Context;

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
			switch (transportStatus)
			{
			case TransportCode::Record:
				if (!context.IsLooping)
				{
					// Initial loop recording
					if (!m_recorder->IsActive)
					{
						// Step 1: press record to start								
						m_recorder->IsActive = true;
						m_masterRecorder->IsActive = m_isRecording;
						transport->Start();
					}
					else
					{
						// Step 2: press record again to stop recording and begin loop
						int switchSamplePos = transport->Context.SamplePosition;
						if (AddTake())
						{
							m_masterRecorder->IsActive = false;
							context.LoopEndSample = switchSamplePos;
							context.IsLooping = true;
							AudioTime switchTime = transport->SamplePositionToAudioTime(switchSamplePos);
						}
					}
				}
				else
				{
					// Toggle recording start / end with next loop wrap
					isRecordPressed = !isRecordPressed;
				}
				break;
			case TransportCode::Locate:
				// Triggered by master loop wrap: delayed handling of 'Record' commands
				if (isRecordPressed)
				{
					isRecordPressed = false;

					if (!m_recorder->IsActive)
					{
						// Step n: start overdub
						m_recorder->IsActive = true;
						m_masterRecorder->IsActive = m_isRecording;
					}
					else
					{
						// Step n + 1: stop overdub and add loop
						int switchSamplePos = transport->Context.SamplePosition;
						if (AddTake())
						{
							m_masterRecorder->IsActive = false;

							if (switchSamplePos > transport->Context.LoopEndSample)
							{
								transport->Context.LoopEndSample = switchSamplePos;
							}
							std::wcout << std::endl;
						}
					}
				}
				break;
			case TransportCode::Start:
			{
				// Drop current recording, stop recording, continue looping
				if (m_recorder->IsActive)
				{
					m_recorder->DropRecording(false);
				}
			}
			break;

			case TransportCode::Stop:
				// Drop current recording, stop recording, exit looping
				transport->Stop();
				if (m_recorder->IsActive)
				{
					m_recorder->DropRecording(false);
				}
				break;
			default:
				break;
			}
		}
	}
	m_device->ProcessingChain->InitShutDown();
	m_transportControl->IsActive = false;
	m_device->Stop();
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

	m_masterRecorder = ObjectFactory::CreateRecorder(processingChain->OutputChannelPairCount * 2, samplesPerTenSecs, samplesPerTenSecs);
	ISampleProcessorPtr masterRecProcessor = nullptr;
	m_masterRecorder->QueryInterface<ISampleProcessor>(&masterRecProcessor);
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

bool Looper::AddTake()
{
	ISampleContainerPtr take = m_recorder->CreateSampleContainer(false, 0, 0);

	if (take != nullptr)
	{
		MixParameter mix;

		ISampleSourcePtr takeSource = ObjectFactory::CreateContainerSource(take);
		takeSource->IsLooping = true;
		m_joiner->AddSource(takeSource, mix);
		return true;
	}
}

void Looper::SaveSession(const wchar_t* pwcszFilenameBase)
{
	ISampleContainerPtr container = m_masterRecorder->CreateSampleContainer(false, 0, 0);

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

bool Looper::get_IsRecording() const
{
	return m_isRecording;
}

void Looper::put_IsRecording(bool value)
{
	m_isRecording = value;
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