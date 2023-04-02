#include "pch.h"
#include "AsioCore.h"
#include "AsioCoreException.h"
#include "AsioError.h"
#include "AsioDebugDriver.h"
#include "AsioDebugDriverGuid.h"
#include <ObjectFactory.h>
#include <IProcessingChain.h>
#include <SampleType.h>
#include <functional>
#include <iostream>
#include <algorithm>

using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;

// static
AsioCore* AsioCore::CreateInstance(REFCLSID clsid)
{
	AsioCore* pCore = new AsioCore();

	if (nullptr == pCore)
	{
		throw AsioCoreException("AsioCore: Not enough memory for ASIO instance.", E_OUTOFMEMORY);
	}

	try
	{
		pCore->Initialize(clsid);
	}
	catch (const std::exception& unmanagedError)
	{
		AsioCoreException exception(unmanagedError.what(), pCore->LastError != S_OK ? pCore->LastError : E_UNEXPECTED);

		delete pCore;

		throw exception;
	}
	catch (AsioCoreException)
	{
		delete pCore;
		throw;
	}
	return pCore;
}

// static
void AsioCore::DeleteInstance(AsioCore* pCore)
{
	if (pCore != nullptr)
	{
		delete pCore;
	}
}

static const ASIOSampleRate InitialSampleRate
{
	{0, 0, 0, 0, 0, 0, 0, 0}
};

AsioCore::AsioCore() :
	m_pDriver(nullptr),
	m_iInputLatency(0),
	m_iOutputLatency(0),
	m_iSampleCount(0),
	m_outputReadySupport(false),
	m_iHwInputCount(0),
	m_iHwOutputCount(0),
	m_iHwPinCount(0),
	m_pHwBufferInfo(nullptr),
	m_iCurrentMonitorInput(-1),
	m_bufferSwitchEventHandler(nullptr),
	m_pCoreCallbacks(nullptr),
	m_lastError(ASE_OK),
	m_sampleRate(InitialSampleRate),
	m_sampleType(-1)
{
	m_supportedBufferSize.Init();
	//memset(&m_sampleRate.ieee, 0, sizeof(ASIOSampleRate::ieee));
}

// virtual
AsioCore::~AsioCore()
{
	CleanUp();
}

void AsioCore::Initialize(REFCLSID clsid)
{
	HRESULT hr;
	IASIO* pDriver = nullptr;

	if (clsid == CLSID_AsioDebugDriver)
	{
		Debug::AsioDebugDriver* pInstance = new Debug::AsioDebugDriver();

		hr = pInstance->QueryInterface(clsid, (void**)&pDriver);
	}
	else
	{
		// clsid is used twice by intention
		hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, clsid, (void**)&pDriver);
	}

	// If the error is E_NOINTERFACE (0x80004002), this may be caused by invalid COM initialization
	// The driver for Steinberg UR-RT2 for example requires STA.
	if (S_OK != hr)
	{
		throw AsioCoreException("AsioCore: Failed to create COM instance for ASIO driver.", hr);
	}

	ASIODriverInfo driverInfo = ASIODriverInfo();
	driverInfo.asioVersion = 2;
	driverInfo.sysRef = nullptr;

	if (ASIOTrue != pDriver->init(&driverInfo))
	{
		char msgBuffer[512];
		ZeroMemory(msgBuffer, _countof(msgBuffer));

		try
		{
			pDriver->getErrorMessage(msgBuffer);
			msgBuffer[_countof(msgBuffer) - 1] = 0;
			pDriver->Release();
		}
		catch (...)
		{
		}

		throw AsioCoreException(msgBuffer[0] != 0 ? msgBuffer : "Driver did not initialise properly");
	}
	else
	{
		m_pDriver = pDriver;
		m_pCoreCallbacks = &AsioCoreCallbacks::Create(this);

		ThrowIfFailed(m_pDriver->getChannels((long*)&m_iHwInputCount, (long*)&m_iHwOutputCount));

		m_iHwPinCount = m_iHwInputCount + m_iHwOutputCount;

		ASIOChannelInfo channelInfo;

		channelInfo.isInput = ASIOTrue;
		channelInfo.channel = 0;

		ThrowIfFailed(m_pDriver->getChannelInfo(&channelInfo));

		m_sampleType = channelInfo.type;

		ThrowIfFailed(m_pDriver->getBufferSize(
			(long*)&m_supportedBufferSize.Minimum, (long*)&m_supportedBufferSize.Maximum,
			(long*)&m_supportedBufferSize.Preferred, (long*)&m_supportedBufferSize.Granularity));

		SelectSampleRate();
	}
}

void AsioCore::CleanUp()
{
	if (nullptr != m_pDriver)
	{
		Stop();
		DisposeBuffers();

		try
		{
			m_pDriver->Release();
		}
		catch (...)
		{
			std::cerr << "Unhandled exception caused by IASIO::Release()" << std::endl;
		}
		m_pDriver = nullptr;
	}
	if (nullptr != m_pCoreCallbacks)
	{
		delete m_pCoreCallbacks;
		m_pCoreCallbacks = nullptr;
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
	if (m_pDriver == nullptr)
	{
		throw AsioCoreException("Driver not initialized");
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
			throw AsioCoreException("Driver does not use supported sample rates 48k or 44.1k.", E_UNEXPECTED);
		}
	}
}

void AsioCore::CreateBuffers(const int inputChannelIds[], int numInputIds, const int outputChannelIds[], int numOutputIds, int sampleCount)
{
	int requestedBufferSize = 0;

	if (sampleCount == UseMaximumSize)
		requestedBufferSize = m_supportedBufferSize.Maximum;
	else if (sampleCount == UsePreferredSize)
		requestedBufferSize = m_supportedBufferSize.Preferred;
	else
		requestedBufferSize = sampleCount;

	DisposeBuffers();

	if (m_iHwPinCount > 0)
	{
		m_pHwBufferInfo = new ASIOBufferInfo[numInputIds + numOutputIds];
		if (nullptr == m_pHwBufferInfo)
		{
			throw AsioCoreException("AsioCore: Not enough memory for array of ASIOBufferInfo.", E_OUTOFMEMORY);
		}

		int iIdx = 0;

		for (int id = 0; id < numInputIds; id++)
		{
			int iInput = inputChannelIds[id];

			m_pHwBufferInfo[iIdx].isInput = ASIOTrue;
			m_pHwBufferInfo[iIdx].channelNum = iInput;
			m_pHwBufferInfo[iIdx].buffers[0] = nullptr;
			m_pHwBufferInfo[iIdx].buffers[1] = nullptr;
			iIdx++;
		}

		for (int id = 0; id < numOutputIds; id++)
		{
			int iOutput = outputChannelIds[id];

			m_pHwBufferInfo[iIdx].isInput = ASIOFalse;
			m_pHwBufferInfo[iIdx].channelNum = iOutput;
			m_pHwBufferInfo[iIdx].buffers[0] = nullptr;
			m_pHwBufferInfo[iIdx].buffers[1] = nullptr;
			iIdx++;
		}

		// Alloc the driver buffers
		ThrowIfFailed(m_pDriver->createBuffers(m_pHwBufferInfo, iIdx, requestedBufferSize, &m_pCoreCallbacks->Callbacks));
		ThrowIfFailed(m_pDriver->getLatencies((long*)&m_iInputLatency, (long*)&m_iOutputLatency));
		m_outputReadySupport = (m_pDriver->outputReady() == ASE_OK);
		m_iSampleCount = requestedBufferSize;

		try
		{
			auto container = ObjectFactory::CreateSampleContainer(m_iSampleCount, std::max<int>(numInputIds, numOutputIds));
			m_processingChain = ObjectFactory::CreateProcessingChain(container);

			CreateInputChannels(0, numInputIds);
			CreateOutputChannels(numInputIds, numOutputIds);
		}
		catch (...)
		{
			DisposeBuffers();
			throw;
		}
	}
}

void AsioCore::CreateInputChannels(int offset, int count)
{
	if (count > 0)
	{
		for (int iIdx = offset; iIdx < offset + count; iIdx++)
		{
			IInputChannelPtr input = ObjectFactory::CreateInputChannel(MapSampleType(m_sampleType),
				m_pHwBufferInfo[iIdx].channelNum,
				m_pHwBufferInfo[iIdx].buffers[0],
				m_pHwBufferInfo[iIdx].buffers[1],
				SampleCount);

			if (nullptr == input)
				throw AsioCoreException("AsioCore: Not enough memory for InputChannel instance.", E_OUTOFMEMORY);

			m_processingChain->AddInput(input);
		}
	}
}

int AsioCore::MapSampleType(ASIOSampleType asioSampleType)
{
	switch (asioSampleType)
	{
	case ASIOSTInt32LSB:
		return Int32LSB;
	case ASIOSTInt24LSB:
		return Int24LSB;
	case ASIOSTFloat32LSB:
		return Float32LSB;
	default:
		throw AsioCoreException("AsioCore: Unsupported sample type.", E_UNEXPECTED);
	}
}

void AsioCore::CreateOutputChannels(int offset, int count)
{
	int pairCount = count / 2;

	if (pairCount > 0)
	{
		for (int pair = 0; pair < pairCount; pair++)
		{
			int iIdx = offset + pair * 2;

			IOutputChannelPairPtr outputPair = ObjectFactory::CreateOutputChannelPair(MapSampleType(m_sampleType),
				m_pHwBufferInfo[iIdx].channelNum,
				m_pHwBufferInfo[iIdx].buffers[0],
				m_pHwBufferInfo[iIdx].buffers[1],
				m_pHwBufferInfo[iIdx + 1].channelNum,
				m_pHwBufferInfo[iIdx + 1].buffers[0],
				m_pHwBufferInfo[iIdx + 1].buffers[1],
				SampleCount);

			if (outputPair == nullptr)
				throw AsioCoreException("AsioCore: Not enough memory for OutputChannelPair instance.", E_OUTOFMEMORY);

			m_processingChain->AddOutputPair(outputPair);
		}
	}
}


void AsioCore::DisposeBuffers()
{
	if (nullptr != m_processingChain)
	{
		m_processingChain->RemoveAllOutputChannels();
		m_processingChain->RemoveAllInputChannels();
	}

	if (nullptr != m_pHwBufferInfo)
	{
		ASIOError ase = m_pDriver->disposeBuffers();

		delete[] m_pHwBufferInfo;
		m_pHwBufferInfo = nullptr;

		ThrowIfFailed(ase);
	}
}


void AsioCore::OnBufferSwitch(long doubleBufferIndex, ASIOBool directProcess) const
{
	// The doubleBufferIndex indicates, 
	// - which output buffer the host should now start to fill
	// - which input buffer is filled with incoming data by the driver
	bool writeSecondHalf = doubleBufferIndex != 0;
	bool readSecondHalf = doubleBufferIndex == 0;

	m_processingChain->OnNextBuffer(writeSecondHalf);

	BufferSwitchEventHandler handler = m_bufferSwitchEventHandler;

	if (nullptr != handler)
	{
		handler(writeSecondHalf);
	}

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

		throw AsioCoreException("AsioCore: ASIO driver reported error.", (int)error);
	}
}

ASIOError AsioCore::get_LastError()
{
	return m_lastError;
}

int AsioCore::get_HardwareInputCount()
{
	return m_iHwInputCount;
}

int AsioCore::get_HardwareOutputCount()
{
	return m_iHwOutputCount;
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
		throw AsioCoreException("Failed to set sample rate");
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

IProcessingChainPtr& AsioCore::get_ProcessingChain()
{
	return m_processingChain;
}

int AsioCore::GetHardwareChannelName(ASIOBool isInput, int index, char* pcBuffer, int max)
{
	ASIOChannelInfo channelInfo;

	ZeroMemory(pcBuffer, max);

	channelInfo.isInput = isInput;
	channelInfo.channel = index;

	ThrowIfFailed(m_pDriver->getChannelInfo(&channelInfo));

	if (strncpy_s(pcBuffer, max, channelInfo.name, _countof(channelInfo.name)))
	{
		return 0;
	}
	return (int)strnlen(pcBuffer, max);
}

int AsioCore::GetHardwareInputName(int index, char* pcBuffer, int max)
{
	return GetHardwareChannelName(ASIOTrue, index, pcBuffer, max);
}

int AsioCore::GetHardwareOutputName(int index, char* pcBuffer, int max)
{
	return GetHardwareChannelName(ASIOFalse, index, pcBuffer, max);
}

