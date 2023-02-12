#include "pch.h"
#include "DriverDescription.h"

using namespace Audio::Asio::Debug;

const char* DriverDescription::ASIODebugDriverName = "ASIODebugDriver";

DriverDescription::DriverDescription() :
	m_pDriverName(nullptr),
	m_lDriverVersion(0),
	m_lInputChannels(0),
	m_lOutputChannels(0),
	m_lInputLatency(0),
	m_lOutputLatency(0),
	m_lMinBufferSize(0),
	m_lMaxBufferSize(0),
	m_lPreferredBufferSize(0),
	m_lGranularity(0),
	m_pClockSources(nullptr),
	m_lNumClockSources(0)
{
	ZeroMemory(m_sampleRate.ieee, sizeof(m_sampleRate.ieee));

	double sampleRate = 44100.0f;

	if (sizeof(sampleRate) != sizeof(m_sampleRate.ieee))
		throw E_UNEXPECTED;

	CopyMemory(m_sampleRate.ieee, &sampleRate, sizeof(sampleRate));

	DriverName = DriverDescription::ASIODebugDriverName;
	DriverVersion = 1;

	InputChannels = 2;
	InputLatency = 20;

	OutputChannels = 2;
	OutputLatency = 12;

	MinBufferSize = 64;
	MaxBufferSize = 4096;
	PreferredBufferSize = 512;
	Granularity = 0;

	ASIOClockSource clockSource;

	clockSource.index = 0;
	clockSource.associatedChannel = 0;
	clockSource.associatedGroup = 0;

	ZeroMemory(clockSource.name, sizeof(clockSource.name));
	strncpy_s(clockSource.name, sizeof(clockSource.name), "Default", 8);

	NumClockSources = 1;
	ClockSource[0] = clockSource;
}

DriverDescription::DriverDescription(const DriverDescription& other) :
	m_pDriverName(nullptr),
	m_lDriverVersion(0),
	m_lInputChannels(0),
	m_lOutputChannels(0),
	m_lInputLatency(0),
	m_lOutputLatency(0),
	m_lMinBufferSize(0),
	m_lMaxBufferSize(0),
	m_lPreferredBufferSize(0),
	m_lGranularity(0),
	m_pClockSources(nullptr),
	m_lNumClockSources(0) 
{
	ZeroMemory(m_sampleRate.ieee, sizeof(m_sampleRate.ieee));

	*this = other;
}

DriverDescription::~DriverDescription()
{
	this->DriverName = nullptr;
	this->NumClockSources = 0;
}

DriverDescription& DriverDescription::operator = (const DriverDescription& other)
{
	DriverName = other.m_pDriverName;

	DriverVersion = other.m_lDriverVersion;
	InputChannels = other.m_lInputChannels;
	OutputChannels = other.m_lOutputChannels;
	InputLatency = other.m_lInputLatency;
	OutputLatency = other.m_lOutputLatency;
	MinBufferSize = other.m_lMinBufferSize;
	MaxBufferSize = other.m_lMaxBufferSize;
	PreferredBufferSize = other.m_lPreferredBufferSize;
	Granularity = other.m_lGranularity;
	SampleRate = other.m_sampleRate;

	NumClockSources = other.m_lNumClockSources;

	for (long cs = 0; cs < NumClockSources; cs++)
		ClockSource[cs] = other.ClockSource[cs];

	return *this;
}

const ASIOClockSource* DriverDescription::GetClockSourceArray(long& numClockSourcesOut) const
{
	numClockSourcesOut = m_lNumClockSources;
	return m_pClockSources;
}

const char* DriverDescription::get_DriverName() const
{
	return m_pDriverName;
}

void DriverDescription::put_DriverName(const char* value)
{
	if (nullptr != m_pDriverName)
	{
		free(m_pDriverName);
		m_pDriverName = nullptr;
	}

	if (nullptr != value)
	{
		if (strlen(value) >= Audio::Asio::Unmanaged::MaxAsioDriverName)
			throw E_INVALIDARG;

		m_pDriverName = _strdup(value);
	}
}

long DriverDescription::get_DriverVersion() const
{
	return m_lDriverVersion;
}

void DriverDescription::put_DriverVersion(long value)
{
	m_lDriverVersion = value;
}

long DriverDescription::get_InputChannels() const
{
	return m_lInputChannels;
}

void DriverDescription::put_InputChannels(long value)
{
	m_lInputChannels = value;
}

long DriverDescription::get_OutputChannels() const
{
	return m_lOutputChannels;
}

void DriverDescription::put_OutputChannels(long value)
{
	m_lOutputChannels = value;
}

long DriverDescription::get_InputLatency() const
{
	return m_lInputLatency;
}

void DriverDescription::put_InputLatency(long value)
{
	m_lInputLatency = value;
}

long DriverDescription::get_OutputLatency() const
{
	return m_lOutputLatency;
}

void DriverDescription::put_OutputLatency(long value)
{
	m_lOutputLatency = value;
}

long DriverDescription::get_MinBufferSize() const
{
	return m_lMinBufferSize;
}

void DriverDescription::put_MinBufferSize(long value)
{
	m_lMinBufferSize = value;
}

long DriverDescription::get_MaxBufferSize() const
{
	return m_lMaxBufferSize;
}

void DriverDescription::put_MaxBufferSize(long value)
{
	m_lMaxBufferSize = value;
}

long DriverDescription::get_PreferredBufferSize() const
{
	return m_lPreferredBufferSize;
}

void DriverDescription::put_PreferredBufferSize(long value)
{
	m_lPreferredBufferSize = value;
}

long DriverDescription::get_Granularity() const
{
	return m_lGranularity;
}

void DriverDescription::put_Granularity(long value)
{
	m_lGranularity = value;
}

ASIOSampleRate DriverDescription::get_SampleRate() const
{
	return m_sampleRate;
}

void DriverDescription::put_SampleRate(ASIOSampleRate value)
{
	m_sampleRate = value;
}

const ASIOClockSource& DriverDescription::get_ClockSource(int index) const
{
	if (index >= this->NumClockSources)
		throw E_INVALIDARG;

	return m_pClockSources[index];
}

void DriverDescription::put_ClockSource(int index, const ASIOClockSource& value)
{
	if (index >= this->NumClockSources)
		throw E_INVALIDARG;

	m_pClockSources[index] = value;
}

long DriverDescription::get_NumClockSources() const
{
	return m_lNumClockSources;
}

void DriverDescription::put_NumClockSources(long value)
{
	if (value != m_lNumClockSources)
	{
		if (nullptr != m_pClockSources)
		{
			m_lNumClockSources = 0;

			delete[] m_pClockSources;
			m_pClockSources = nullptr;
		}
		if (0 < value)
		{
			m_pClockSources = new ASIOClockSource[value];

			if (nullptr == m_pClockSources)
				throw E_OUTOFMEMORY;
		}
		m_lNumClockSources = value;
	}
}

