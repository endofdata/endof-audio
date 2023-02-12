#include "pch.h"
#define INITGUID
#include "AsioDebugDriverGuid.h"
#undef INITGUID
#include "AsioDebugDriver.h"

using namespace Audio::Asio::Debug;

AsioDebugDriver::AsioDebugDriver() :
	m_description(),
	m_pSysHandle(nullptr),
	m_isStarted(false),
	m_selectedClockSource(-1),
	m_pCallbacks(nullptr),
	m_lNumChannels(0),
	m_lBufferSize(0),
	m_pTheBigBufferForAll(nullptr),
	m_hThread(nullptr),
	m_dwThreadId(0),
	m_hevStop(nullptr),
	m_dwInterruptInterval(100)
{
	ZeroMemory(m_acErrorMessage, sizeof(m_acErrorMessage));

	m_samplePos.hi = 0;
	m_samplePos.lo = 0;

	m_timeStamp.hi = 0;
	m_timeStamp.lo = 0;

	m_ulRef = 0;
}

AsioDebugDriver::AsioDebugDriver(const DriverDescription& description) :
	m_description(description),
	m_pSysHandle(nullptr),
	m_isStarted(false),
	m_selectedClockSource(-1),
	m_pCallbacks(nullptr),
	m_lNumChannels(0),
	m_lBufferSize(0),
	m_pTheBigBufferForAll(nullptr),
	m_hThread(nullptr),
	m_dwThreadId(0),
	m_hevStop(nullptr),
	m_dwInterruptInterval(100)
{
	ZeroMemory(m_acErrorMessage, sizeof(m_acErrorMessage));

	m_samplePos.hi = 0;
	m_samplePos.lo = 0;

	m_timeStamp.hi = 0;
	m_timeStamp.lo = 0;

	m_ulRef = 0;
}

AsioDebugDriver::~AsioDebugDriver()
{
	stop();

	disposeBuffers();
}

HRESULT AsioDebugDriver::QueryInterface(REFIID riid, void **ppvObject)
{
	if (nullptr == ppvObject)
		return E_POINTER;

	HRESULT hr = S_OK;

	*ppvObject = nullptr;

	if (riid == IID_IUnknown)
		*ppvObject = dynamic_cast<IUnknown*>(this);
	else if (riid == CLSID_AsioDebugDriver)
		*ppvObject = dynamic_cast<IASIO*>(this);
	else
		hr = E_NOINTERFACE;

	if (SUCCEEDED(hr))
		(static_cast<IUnknown*>(*ppvObject))->AddRef();

	return hr;
}

ULONG AsioDebugDriver::AddRef(void)
{
	return InterlockedIncrement(&m_ulRef);
}

ULONG AsioDebugDriver::Release(void)
{
	ULONG ulRet = InterlockedDecrement(&m_ulRef);

	if (0 == ulRet)
		delete this;

	return ulRet;
}

DWORD AsioDebugDriver::get_InterruptInterval() const
{
	return m_dwInterruptInterval;
}

void AsioDebugDriver::set_InterruptInterval(DWORD value)
{
	m_dwInterruptInterval = value;
}

ASIOBool AsioDebugDriver::init(void *sysHandle)
{
	m_pSysHandle = sysHandle;

	return ASIOTrue;
}

void AsioDebugDriver::getDriverName(char *name)
{
	const char* pcszOwnName;

	if (nullptr != m_description.DriverName)
		pcszOwnName = m_description.DriverName;
	else
		pcszOwnName = "ASIO Debug Driver";

	strncpy_s(name, Audio::Asio::Unmanaged::MaxAsioDriverName, pcszOwnName, Audio::Asio::Unmanaged::MaxAsioDriverName - 1);
}


long AsioDebugDriver::getDriverVersion()
{
	return m_description.DriverVersion;
}


void AsioDebugDriver::getErrorMessage(char *string)
{
	strncpy_s(string, Audio::Asio::Unmanaged::MaxAsioErrorMessage, m_acErrorMessage, Audio::Asio::Unmanaged::MaxAsioErrorMessage - 1);
}

DWORD AsioDebugDriver::RunInterruptSimulator()
{
	DWORD result = S_OK;
	long lBufferIndex = 0;

	while (WaitForSingleObject(m_hevStop, m_dwInterruptInterval) == WAIT_TIMEOUT)
	{
		ASIOCallbacks* pCallbacks = m_pCallbacks;

		if (nullptr != pCallbacks)
		{
			m_pCallbacks->bufferSwitch(lBufferIndex, ASIOTrue);
			lBufferIndex = (lBufferIndex == 0) ? 1 : 0;
		}
		else
		{
			result = E_UNEXPECTED;
			break;
		}
	}
	return result;
}

DWORD _stdcall AsioDebugDriver::InterruptSimulatorThreadEntry(LPVOID lpvThis)
{
	DWORD dwExitCode = E_UNEXPECTED;

	try
	{
		dwExitCode = (static_cast<AsioDebugDriver*>(lpvThis))->RunInterruptSimulator();
	}
	catch (...)
	{
		dwExitCode = E_ABORT;
	}
	ExitThread(dwExitCode);
}

ASIOError AsioDebugDriver::start()
{
	ASIOError result = ASE_InvalidMode;

	if (!m_isStarted)
	{
		m_isStarted = true;
		m_hevStop = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		m_hThread = CreateThread(nullptr, 0, InterruptSimulatorThreadEntry, this, 0, &m_dwThreadId);

		result = ASE_OK;
	}
	return result;
}


ASIOError AsioDebugDriver::stop()
{
	ASIOError result = ASE_InvalidMode;

	if (m_isStarted)
	{
		SetEvent(m_hevStop);

		if (WAIT_OBJECT_0 == WaitForSingleObject(m_hThread, 5000))
		{
			CloseHandle(m_hevStop);
			CloseHandle(m_hThread);

			m_isStarted = false;

			result = ASE_OK;
		}
		else
			result = ASE_HWMalfunction;
	}
	else
		result = ASE_OK;

	return result;
}


ASIOError AsioDebugDriver::getChannels(long *numInputChannels, long *numOutputChannels)
{
	if (nullptr != numInputChannels)
		*numInputChannels = m_description.InputChannels;

	if (nullptr != numOutputChannels)
		*numOutputChannels = m_description.OutputChannels;

	return ASE_OK;
}


ASIOError AsioDebugDriver::getLatencies(long *inputLatency, long *outputLatency)
{
	if (nullptr != inputLatency)
		*inputLatency = m_description.InputLatency;

	if (nullptr != outputLatency)
		*outputLatency = m_description.OutputLatency;

	return ASE_OK;
}


ASIOError AsioDebugDriver::getBufferSize(long *minSize, long *maxSize,
	long *preferredSize, long *granularity)
{
	if (nullptr != minSize)
		*minSize = m_description.MinBufferSize;

	if (nullptr != maxSize)
		*maxSize = m_description.MaxBufferSize;

	if (nullptr != preferredSize)
		*preferredSize = m_description.PreferredBufferSize;

	return ASE_OK;
}


ASIOError AsioDebugDriver::canSampleRate(ASIOSampleRate sampleRate)
{
	return memcmp(sampleRate.ieee, m_description.SampleRate.ieee, sizeof(sampleRate.ieee))? ASE_NotPresent : ASE_OK;
}

ASIOError AsioDebugDriver::getSampleRate(ASIOSampleRate *sampleRate)
{
	if(nullptr != sampleRate)
		*sampleRate = m_description.SampleRate;

	return ASE_OK;
}

ASIOError AsioDebugDriver::setSampleRate(ASIOSampleRate sampleRate)
{
	m_description.SampleRate = sampleRate;

	return ASE_OK;
}


ASIOError AsioDebugDriver::getClockSources(ASIOClockSource *clocks, long *numSources)
{
	if (nullptr == clocks)
		return ASE_InvalidParameter;

	if (nullptr == numSources)
		return ASE_InvalidParameter;

	long lDefinedSources = m_description.NumClockSources;
	long lFoundSources = 0;

	for (long s = 0; s < *numSources; s++)
	{
		long sourceIndex = clocks[s].index;

		for (long d = 0; d < lDefinedSources; d++)
		{
			if (m_description.ClockSource[d].index == sourceIndex)
			{
				clocks[lFoundSources] = m_description.ClockSource[d];
				clocks[lFoundSources].isCurrentSource = (sourceIndex == m_selectedClockSource);

				lFoundSources++;

				break;
			}
		}
	}

	if (nullptr != numSources)
		*numSources = lFoundSources;

	return ASE_OK;
}


ASIOError AsioDebugDriver::setClockSource(long reference)
{
	if (reference < 0 || reference >= m_description.NumClockSources)
		return ASE_InvalidParameter;

	m_selectedClockSource = reference;

	return ASE_OK;
}


ASIOError AsioDebugDriver::getSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp)
{
	if (nullptr != sPos)
		*sPos = m_samplePos;

	if (nullptr != tStamp)
		*tStamp = m_timeStamp;

	return ASE_OK;
}


ASIOError AsioDebugDriver::getChannelInfo(ASIOChannelInfo *info)
{
	if (nullptr == info)
		return ASE_InvalidParameter;

	ASIOError result = ASE_InvalidParameter;

	if (info->isInput)
	{
		if (info->channel >= 0 && info->channel < m_description.InputChannels)
		{
			info->channelGroup = 0;
			info->type = ASIOSTInt32LSB;
			snprintf(info->name, sizeof(info->name), "Input %u", info->channel + 1);

			result = ASE_OK;
		}
	}
	else
	{
		if (info->channel >= 0 && info->channel < m_description.OutputChannels)
		{
			info->channelGroup = 1;
			info->type = ASIOSTInt32LSB;
			snprintf(info->name, sizeof(info->name), "Output %u", info->channel + 1);

			result = ASE_OK;
		}
	}
	return result;
}


ASIOError AsioDebugDriver::createBuffers(ASIOBufferInfo *bufferInfos, long numChannels,
	long bufferSize, ASIOCallbacks *callbacks)
{
	if (nullptr == bufferInfos)
		return ASE_InvalidParameter;

	if (numChannels <= 0 || numChannels > m_description.InputChannels + m_description.OutputChannels)
		return ASE_InvalidParameter;

	if (bufferSize < m_description.MinBufferSize || bufferSize > m_description.MaxBufferSize)
		return ASE_InvalidParameter;

	disposeBuffers();

	ASIOError result = ASE_NotPresent;

	m_pTheBigBufferForAll = new int[bufferSize * 2 * numChannels];

	if (nullptr == m_pTheBigBufferForAll)
		result = ASE_NoMemory;
	else
	{
		for (long b = 0; b < numChannels; b++)
		{
			int* pDoubleBuffer = &m_pTheBigBufferForAll[b * bufferSize * 2];

			bufferInfos[b].buffers[0] = &pDoubleBuffer[0];
			bufferInfos[b].buffers[1] = &pDoubleBuffer[bufferSize];
		}
		m_pCallbacks = callbacks;
		m_lBufferSize = bufferSize;
		m_lNumChannels = numChannels;

		result = ASE_OK;
	}
	return result;
}


ASIOError AsioDebugDriver::disposeBuffers()
{
	if (nullptr != m_pTheBigBufferForAll)
	{
		delete[] m_pTheBigBufferForAll;
		m_pTheBigBufferForAll = nullptr;
	}

	m_pCallbacks = nullptr;
	m_lBufferSize = 0;
	m_lNumChannels = 0;

	return ASE_OK;
}


ASIOError AsioDebugDriver::controlPanel()
{
	return ASE_NotPresent;
}


ASIOError AsioDebugDriver::future(long selector, void *opt)
{
	return ASE_NotPresent;
}


ASIOError AsioDebugDriver::outputReady()
{
	return ASE_OK;
}


