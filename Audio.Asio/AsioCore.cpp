#include "pch.h"
#include "AsioCore.h"
#include "AsioCoreException.h"
#include "AsioDebugDriver.h"
#include "AsioDebugDriverGuid.h"
#include "InputInt32Channel.h"
#include "InputInt24Channel.h"
#include "InputFloat32Channel.h"
#include "OutputInt32ChannelPair.h"
#include "OutputInt24ChannelPair.h"
#include "OutputFloat32ChannelPair.h"

using namespace Audio::Asio;
using namespace Audio::Foundation::Abstractions;

// static
AsioCore* AsioCore::CreateInstance(REFCLSID clsid, int iSamplesPerBuffer)
{
	AsioCore* pCore = new AsioCore(clsid);

	if (NULL == pCore)
	{
		throw gcnew AsioCoreException("AsioCore: Not enough memory for ASIO instance.", E_OUTOFMEMORY);
	}

	pCore->CreateBuffers(iSamplesPerBuffer);

	return pCore;
}


AsioCore::AsioCore(REFCLSID clsid) :
	m_pDriver(NULL),
	m_iInputLatency(0),
	m_iOutputLatency(0),
	m_iSampleCount(0),
	m_outputReadySupport(false),
	m_iHwInputCount(0),
	m_iHwOutputCount(0),
	m_iHwPinCount(0),
	m_pHwBufferInfo(NULL),
	m_iInputChannels(0),
	m_pInputChannels(NULL),
	m_iOutputChannelPairs(0),
	m_pOutputChannelPairs(NULL),
	m_iCurrentMonitorInput(-1),
	m_bufferSwitchEventHandler(NULL),
	m_pCoreCallbacks(NULL),
	m_sampleType(-1)
{
	m_supportedBufferSize.Init();
	Initialize(clsid);
}

// virtual
AsioCore::~AsioCore()
{
	CleanUp();
}

void AsioCore::Initialize(REFCLSID clsid)
{
	HRESULT hr;

	if (clsid == CLSID_AsioDebugDriver)
	{
		Debug::AsioDebugDriver* pDriver = new Debug::AsioDebugDriver();

		hr = pDriver->QueryInterface(clsid, (void**)&m_pDriver);
	}
	else
	{
		// clsid is used twice by intention
		hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, clsid, (void**)&m_pDriver);
	}

	// If the error is E_NOINTERFACE (0x80004002), this may be caused by invalid COM initialization
	// The driver for Steinberg UR-RT2 for example requires STA.
	if (S_OK != hr)
	{
		throw gcnew AsioCoreException("AsioCore: Failed to create COM instance for ASIO driver.", hr);
	}

	ASIODriverInfo driverInfo = ASIODriverInfo();
	driverInfo.asioVersion = 2;
	driverInfo.sysRef = NULL;

	if (ASIOTrue != m_pDriver->init(&driverInfo))
	{
		CleanUp();
		throw gcnew AsioCoreException("Driver did not initialise properly.");
	}
	else
	{
		m_pCoreCallbacks = &AsioCoreCallbacks::Create(this);

		ASIOChannelInfo channelInfo = ASIOChannelInfo();
		channelInfo.isInput = ASIOTrue;
		channelInfo.channel = 0;

		ThrowIfFailed(m_pDriver->getChannelInfo(&channelInfo));
		switch(channelInfo.type)
		{
		case ASIOSTInt32LSB:
		case ASIOSTInt24LSB:
		case ASIOSTFloat32LSB:
			break;
		default:		
			CleanUp();
			throw gcnew AsioCoreException("Driver does not use supported sample formats 'Int24/LSB', 'Int32/LSB' or 'Float32/LSB'.", E_UNEXPECTED);
		}
		m_sampleType = channelInfo.type;

		ThrowIfFailed(m_pDriver->getChannels((long*)&m_iHwInputCount, (long*)&m_iHwOutputCount));

		m_iHwPinCount = m_iHwInputCount + m_iHwOutputCount;

		ThrowIfFailed(m_pDriver->getBufferSize(
			(long*)&m_supportedBufferSize.Minimum, (long*)&m_supportedBufferSize.Maximum,
			(long*)&m_supportedBufferSize.Preferred, (long*)&m_supportedBufferSize.Granularity));

		SelectSampleRate();
	}
}

void AsioCore::CleanUp()
{
	if (NULL != m_pDriver)
	{
		Stop();
		DisposeBuffers();
		m_pDriver->Release();
		m_pDriver = NULL;
	}
	if (NULL != m_pCoreCallbacks)
	{
		delete m_pCoreCallbacks;
		m_pCoreCallbacks = NULL;
	}
}

void AsioCore::Start()
{
	ThrowIfFailed(m_pDriver->start());
}

void AsioCore::Stop()
{
	ThrowIfFailed(m_pDriver->stop());
}

void AsioCore::ShowControlPanel()
{
	//ThrowIfFailed(m_pDriver->controlPanel());
	m_pDriver->controlPanel();
}


void AsioCore::GetDriverName(char* pcNameOut)
{
	m_pDriver->getDriverName(pcNameOut);
}

int AsioCore::GetDriverVersion()
{
	return (int)m_pDriver->getDriverVersion();
}

void AsioCore::SelectSampleRate()
{
	if (m_pDriver == NULL)
	{
		throw gcnew AsioCoreException("Driver not initialized");
	}

	ThrowIfFailed(m_pDriver->getSampleRate(&m_sampleRate));

	if (SampleRate == 0.0)
	{
		double supportedRates[] = { 48000.0, 44100.0, 0.0 };

		for (int r = 0; supportedRates[r] != 0.0; r++)
		{
			ASIOSampleRate sampleRate;
			*(double*)sampleRate.ieee = supportedRates[r];

			if (m_pDriver->canSampleRate(sampleRate) == ASE_OK)
			{
				SampleRate = supportedRates[r];
				break;
			}
		}

		if (SampleRate == 0.0)
		{
			throw gcnew AsioCoreException("Driver does not use supported sample rates 48k or 44.1k.", E_UNEXPECTED);
		}
	}
}

void AsioCore::CreateBuffers(int iSampleCount)
{
	int requestedBufferSize = 0;

	if (iSampleCount == UseMaximumSize)
		requestedBufferSize = m_supportedBufferSize.Maximum;
	else if (iSampleCount == UsePreferredSize)
		requestedBufferSize = m_supportedBufferSize.Preferred;
	else
		requestedBufferSize = iSampleCount;

	DisposeBuffers();

	if (m_iHwPinCount > 0)
	{
		m_pHwBufferInfo = new ASIOBufferInfo[m_iHwPinCount];
		if (NULL == m_pHwBufferInfo)
			throw gcnew AsioCoreException("AsioCore: Not enough memory for array of ASIOBufferInfo.", E_OUTOFMEMORY);

		int iIdx = 0;

		for (int iInput = 0; iInput < m_iHwInputCount; iInput++)
		{
			m_pHwBufferInfo[iIdx].isInput = ASIOTrue;
			m_pHwBufferInfo[iIdx].channelNum = iInput;
			m_pHwBufferInfo[iIdx].buffers[0] = NULL;
			m_pHwBufferInfo[iIdx].buffers[1] = NULL;
			iIdx++;
		}

		for (int iOutput = 0; iOutput < m_iHwOutputCount; iOutput++)
		{
			m_pHwBufferInfo[iIdx].isInput = ASIOFalse;
			m_pHwBufferInfo[iIdx].channelNum = iOutput;
			m_pHwBufferInfo[iIdx].buffers[0] = NULL;
			m_pHwBufferInfo[iIdx].buffers[1] = NULL;
			iIdx++;
		}

		// Alloc the driver buffers
		ThrowIfFailed(m_pDriver->createBuffers(m_pHwBufferInfo, m_iHwPinCount, requestedBufferSize, &m_pCoreCallbacks->Callbacks));
		ThrowIfFailed(m_pDriver->getLatencies((long*)&m_iInputLatency, (long*)&m_iOutputLatency));
		m_outputReadySupport = (m_pDriver->outputReady() == ASE_OK);
		m_iSampleCount = requestedBufferSize;

		try
		{
			CreateInputChannels();
			CreateOutputChannels();
		}
		catch (...)
		{
			DisposeBuffers();
			throw;
		}
	}
}

void AsioCore::CreateInputChannels()
{
	if (m_iHwInputCount > 0)
	{
		// each input channel splits the signal to two sample buffers
		m_pInputChannels = (IInputChannel**)new IInputChannel*[m_iHwInputCount];
		if (NULL == m_pInputChannels)
			throw gcnew AsioCoreException("AsioCore: Not enough memory for InputChannel array.", E_OUTOFMEMORY);

		ZeroMemory(m_pInputChannels, sizeof(IInputChannel*) * m_iHwInputCount);

		m_iInputChannels = 0;

		for (int iIdx = 0; iIdx < m_iHwInputCount; iIdx++)
		{
			switch(m_sampleType)
			{
			case ASIOSTInt32LSB:
				m_pInputChannels[m_iInputChannels] = dynamic_cast<IInputChannel*>(new Audio::Asio::InputInt32Channel(
					m_pHwBufferInfo[iIdx].channelNum,
					(int*)m_pHwBufferInfo[iIdx].buffers[0],
					(int*)m_pHwBufferInfo[iIdx].buffers[1],
					SampleCount));
				break;
			case ASIOSTInt24LSB:
				m_pInputChannels[m_iInputChannels] = dynamic_cast<IInputChannel*>(new Audio::Asio::InputInt24Channel(
					m_pHwBufferInfo[iIdx].channelNum,
					(byte*)m_pHwBufferInfo[iIdx].buffers[0],
					(byte*)m_pHwBufferInfo[iIdx].buffers[1],
					SampleCount));
				break;
			case ASIOSTFloat32LSB:
				m_pInputChannels[m_iInputChannels] = dynamic_cast<IInputChannel*>(new Audio::Asio::InputFloat32Channel(
					m_pHwBufferInfo[iIdx].channelNum,
					(float*)m_pHwBufferInfo[iIdx].buffers[0],
					(float*)m_pHwBufferInfo[iIdx].buffers[1],
					SampleCount));
				break;
			default:
				throw gcnew AsioCoreException("AsioCore: Unsupported sample type.", E_UNEXPECTED);
			}
			
			if (NULL == m_pInputChannels[m_iInputChannels])
				throw gcnew AsioCoreException("AsioCore: Not enough memory for InputChannel instance.", E_OUTOFMEMORY);

			m_iInputChannels++;
		}
	}
}

void AsioCore::CreateOutputChannels()
{
	if (m_iHwOutputCount > 0)
	{
		// each output channel is linked to two hardware outputs
		m_pOutputChannelPairs = (IOutputChannelPair**)new IOutputChannelPair*[m_iHwOutputCount / 2];
		if (NULL == m_pOutputChannelPairs)
			throw gcnew AsioCoreException("AsioCore: Not enough memory for OutputChannelPair array.", E_OUTOFMEMORY);

		ZeroMemory(m_pOutputChannelPairs, sizeof(IOutputChannelPair*) * m_iHwOutputCount / 2);

		m_iOutputChannelPairs = 0;

		for (int iIdx = m_iHwInputCount; iIdx < m_iHwPinCount - 1; iIdx += 2)
		{
			switch (m_sampleType)
			{
			case ASIOSTInt32LSB:
				m_pOutputChannelPairs[m_iOutputChannelPairs] = dynamic_cast<IOutputChannelPair*>(new ::Audio::Asio::OutputInt32ChannelPair(
					m_pHwBufferInfo[iIdx].channelNum,
					(int*)m_pHwBufferInfo[iIdx].buffers[0],
					(int*)m_pHwBufferInfo[iIdx].buffers[1],
					m_pHwBufferInfo[iIdx + 1].channelNum,
					(int*)m_pHwBufferInfo[iIdx + 1].buffers[0],
					(int*)m_pHwBufferInfo[iIdx + 1].buffers[1],
					SampleCount));
				break;
			case ASIOSTInt24LSB:
				m_pOutputChannelPairs[m_iOutputChannelPairs] = dynamic_cast<IOutputChannelPair*>(new ::Audio::Asio::OutputInt24ChannelPair(
					m_pHwBufferInfo[iIdx].channelNum,
					(byte*)m_pHwBufferInfo[iIdx].buffers[0],
					(byte*)m_pHwBufferInfo[iIdx].buffers[1],
					m_pHwBufferInfo[iIdx + 1].channelNum,
					(byte*)m_pHwBufferInfo[iIdx + 1].buffers[0],
					(byte*)m_pHwBufferInfo[iIdx + 1].buffers[1],
					SampleCount));
				break;			
			case ASIOSTFloat32LSB:
				m_pOutputChannelPairs[m_iOutputChannelPairs] = dynamic_cast<IOutputChannelPair*>(new ::Audio::Asio::OutputFloat32ChannelPair(
					m_pHwBufferInfo[iIdx].channelNum,
					(float*)m_pHwBufferInfo[iIdx].buffers[0],
					(float*)m_pHwBufferInfo[iIdx].buffers[1],
					m_pHwBufferInfo[iIdx + 1].channelNum,
					(float*)m_pHwBufferInfo[iIdx + 1].buffers[0],
					(float*)m_pHwBufferInfo[iIdx + 1].buffers[1],
					SampleCount));
				break;
			default:
				throw gcnew AsioCoreException("AsioCore: Unsupported sample type.", E_UNEXPECTED);
			}
			if (NULL == m_pOutputChannelPairs[m_iOutputChannelPairs])
				throw gcnew AsioCoreException("AsioCore: Not enough memory for OutputChannelPair instance.", E_OUTOFMEMORY);

			m_iOutputChannelPairs++;
		}
	}
}


void AsioCore::DisposeBuffers()
{
	DisposeOutputChannels();

	DisposeInputChannels();

	if (NULL != m_pHwBufferInfo)
	{
		ASIOError ase = m_pDriver->disposeBuffers();

		delete[] m_pHwBufferInfo;
		m_pHwBufferInfo = NULL;

		ThrowIfFailed(ase);
	}
}

void AsioCore::DisposeInputChannels()
{
	if (NULL != m_pInputChannels)
	{
		for (int i = 0; i < m_iHwInputCount; i++)
		{
			if (NULL != m_pInputChannels[i])
			{
				switch (m_sampleType)
				{
				case ASIOSTInt32LSB:
					delete dynamic_cast<InputInt32Channel*>(m_pInputChannels[i]);
					break;
				case ASIOSTInt24LSB:
					delete dynamic_cast<InputInt24Channel*>(m_pInputChannels[i]);
					break;
				case ASIOSTFloat32LSB:
					delete dynamic_cast<InputFloat32Channel*>(m_pInputChannels[i]);
					break;
				}
			}
		}
		delete[] m_pInputChannels;
		m_pInputChannels = NULL;
	}
}

void AsioCore::DisposeOutputChannels()
{
	if (NULL != m_pOutputChannelPairs)
	{
		for (int i = 0; i < m_iOutputChannelPairs; i++)
		{
			if (NULL != m_pOutputChannelPairs[i])
			{
				switch (m_sampleType)
				{
				case ASIOSTInt32LSB:
					delete dynamic_cast<OutputInt32ChannelPair*>(m_pOutputChannelPairs[i]);
					break;
				case ASIOSTInt24LSB:
					delete dynamic_cast<OutputInt24ChannelPair*>(m_pOutputChannelPairs[i]);
					break;
				case ASIOSTFloat32LSB:
					delete dynamic_cast<OutputFloat32ChannelPair*>(m_pOutputChannelPairs[i]);
					break;
				}
			}
		}
		delete[] m_pOutputChannelPairs;
		m_pOutputChannelPairs = NULL;
	}
}

void AsioCore::SetInputMonitoring(int iInputChannel, int iOutputPair)
{
	if (m_iCurrentMonitorInput >= 0)
	{
		m_pInputChannels[m_iCurrentMonitorInput]->Monitor = NULL;
		m_iCurrentMonitorInput = -1;
	}
	if (0 <= iInputChannel && iInputChannel < m_iInputChannels)
	{
		if (0 <= iOutputPair && iOutputPair < m_iOutputChannelPairs)
		{
			m_pInputChannels[iInputChannel]->Monitor = m_pOutputChannelPairs[iOutputPair];
			m_iCurrentMonitorInput = iInputChannel;
		}
	}
}

void AsioCore::OnBufferSwitch(long doubleBufferIndex, ASIOBool directProcess) const
{
	// The doubleBufferIndex indicates, 
	// - which output buffer the host should now start to fill
	// - which input buffer is filled with incoming data by the driver
	bool writeSecondHalf = doubleBufferIndex != 0;
	bool readSecondHalf = doubleBufferIndex == 0;

	for (int iOutput = 0; iOutput < m_iOutputChannelPairs; iOutput++)
	{
		IOutputChannelPair* pChannelPair = m_pOutputChannelPairs[iOutput];
		pChannelPair->Swap(writeSecondHalf);
	}

	for (int iInput = 0; iInput < m_iInputChannels; iInput++)
	{
		IInputChannel* pChannel = m_pInputChannels[iInput];
		pChannel->Swap(readSecondHalf);
		pChannel->Send();
	}

	BufferSwitchEventHandler handler = m_bufferSwitchEventHandler;

	if (NULL != handler)
	{
		handler(writeSecondHalf);
	}
	//for(int iOutput = 0; iOutput < m_iOutputChannelPairs; iOutput++)
	//{
	//	Audio::Asio::OutputChannelPair* pChannelPair = m_pOutputChannelPairs[iOutput];
	//	pChannelPair->Flush();
	//}

	if (m_outputReadySupport)
	{
		m_pDriver->outputReady();
	}
}

ASIOTime* AsioCore::OnBufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	OnBufferSwitch(doubleBufferIndex, directProcess);
	return params;
}

void AsioCore::OnSampleRateDidChange(ASIOSampleRate rate)
{
	m_sampleRate = rate;
}

long AsioCore::OnAsioMessage(long selector, long value, void* message, double* opt)
{
	switch (selector)
	{
	case kAsioSelectorSupported:
		switch (value)
		{
		case kAsioEngineVersion:
			return 1;
		case kAsioResetRequest:
			return 0;
		case kAsioBufferSizeChange:
			return 0;
		case kAsioResyncRequest:
			return 0;
		case kAsioLatenciesChanged:
			return 0;
		case kAsioSupportsTimeInfo:
			return 1;
		case kAsioSupportsTimeCode:
			return 1;
		}
		break;
	case kAsioEngineVersion:
		return 2;
	case kAsioResetRequest:
		// e.g. changes in ASIO4All config dlg
		return 1;
	case kAsioBufferSizeChange:
		return 0;
	case kAsioResyncRequest:
		return 0;
	case kAsioLatenciesChanged:
		return 0;
	case kAsioSupportsTimeInfo:
		return 0;
	case kAsioSupportsTimeCode:
		return 0;
	}
	return 0;
}

void AsioCore::ThrowIfFailed(ASIOError error)
{
	if (ASE_OK != error)
	{
		m_lastError = error;
		throw gcnew AsioCoreException("AsioCore: ASIO driver reported error.", (int)error);
	}
}

ASIOError AsioCore::get_LastError()
{
	return m_lastError;
}

int AsioCore::get_InputChannelCount()
{
	return m_iInputChannels;
}

IInputChannel* AsioCore::get_InputChannel(int iChannel)
{
	IInputChannel* value = NULL;

	if (0 <= iChannel && iChannel < m_iInputChannels)
		value = m_pInputChannels[iChannel];

	return value;
}

int AsioCore::get_OutputChannelPairCount()
{
	return m_iOutputChannelPairs;
}

IOutputChannelPair* AsioCore::get_OutputChannelPair(int iChannel)
{
	IOutputChannelPair* value = NULL;

	if (0 <= iChannel && iChannel < m_iOutputChannelPairs)
		value = m_pOutputChannelPairs[iChannel];

	return value;
}

int AsioCore::get_SampleCount()
{
	return m_iSampleCount;
}

double AsioCore::get_SampleRate()
{
	return *reinterpret_cast<double*>(&m_sampleRate);
}

void AsioCore::put_SampleRate(double rate)
{
	ASIOSampleRate& asioSampleRate = *reinterpret_cast<ASIOSampleRate*>(&rate);

	ThrowIfFailed(m_pDriver->canSampleRate(asioSampleRate));
	ThrowIfFailed(m_pDriver->setSampleRate(asioSampleRate));
	m_pDriver->getSampleRate(&m_sampleRate);

	if (SampleRate != rate)
	{
		throw gcnew AsioCoreException(System::String::Format("Failed to set sample rate to {0:N1}", rate));
	}
}


ASIOSampleType AsioCore::get_SampleType()
{
	return m_sampleType;
}

int AsioCore::get_SampleSize()
{
	return 4; // 32 Bits => 4 bytes / sample
}

BufferSwitchEventHandler AsioCore::get_BufferSwitchEventHandler()
{
	return m_bufferSwitchEventHandler;
}

void AsioCore::put_BufferSwitchEventHandler(BufferSwitchEventHandler value)
{
	m_bufferSwitchEventHandler = value;
}
