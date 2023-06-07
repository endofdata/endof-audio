#include "pch.h"
#include "LooperConfig.h"
#include <AsioCore.h>

LooperConfig::LooperConfig() :
	m_numInputIds(0),
	m_numOutputIds(0),
	m_outputSaturation(0.0),
	m_sampleCount(Audio::Asio::Unmanaged::AsioCore::UsePreferredSize),
	m_midiInput(0)
{
	std::memset(&m_asioDevice, 0, sizeof(m_asioDevice));
	std::memset(&m_inputChannelIds, 0, sizeof(m_inputChannelIds));
	std::memset(&m_outputChannelIds, 0, sizeof(m_outputChannelIds));
}

IMPLEMENT_IUNKNOWN(LooperConfig)

void* LooperConfig::GetInterface(const IID& riid)
{
	if (riid == _uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (riid == _uuidof(ILooperConfig))
	{
		return dynamic_cast<ILooperConfig*>(this);
	}
	return nullptr;
}

void LooperConfig::AddInputChannel(int id)
{
	if (m_numInputIds >= LOOPER_MAX_INPUT_CHANNELS)
	{
		throw std::invalid_argument("No more input channel slots.");
	}
	m_inputChannelIds[m_numInputIds++] = id;
}

void LooperConfig::AddOutputChannel(int id)
{
	if (m_numOutputIds >= LOOPER_MAX_OUTPUT_CHANNELS)
	{
		throw std::invalid_argument("No more output channel slots.");
	}
	m_outputChannelIds[m_numOutputIds++] = id;
}

void LooperConfig::AddInputChannelList(int idList[], size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		AddInputChannel(idList[i]);
	}
}

void LooperConfig::AddOutputChannelList(int idList[], size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		AddOutputChannel(idList[i]);
	}
}

const wchar_t* LooperConfig::get_Name() const
{
	return m_name.c_str();
}

void LooperConfig::put_Name(const wchar_t* value)
{
	m_name.assign(value);
}

unsigned int LooperConfig::get_MidiInput() const
{
	return m_midiInput;
}

void LooperConfig::put_MidiInput(unsigned int value)
{
	m_midiInput = value;
}


const IID& LooperConfig::get_AsioDevice() const
{
	return m_asioDevice;
}

void LooperConfig::put_AsioDevice(const IID& value)
{
	m_asioDevice = value;
}

int LooperConfig::get_InputChannel(int index) const
{
	if (index < 0 || index >= m_numInputIds)
	{
		throw std::invalid_argument("Index out of range.");
	}
	return m_inputChannelIds[index];
}

void LooperConfig::put_InputChannel(int index, int value)
{
	if (index < 0 || index >= m_numInputIds)
	{
		throw std::invalid_argument("Index out of range.");
	}
	m_inputChannelIds[index] = value;
}

const int* LooperConfig::get_InputChannelList() const
{
	return m_inputChannelIds;
}

size_t LooperConfig::get_InputChannelCount() const
{
	return m_numInputIds;
}

int LooperConfig::get_OutputChannel(int index) const
{
	if (index < 0 || index >= m_numOutputIds)
	{
		throw std::invalid_argument("Index out of range.");
	}
	return m_outputChannelIds[index];
}


void LooperConfig::put_OutputChannel(int index, int value)
{
	if (index < 0 || index >= m_numOutputIds)
	{
		throw std::invalid_argument("Index out of range.");
	}
	m_outputChannelIds[index] = value;
}

const int* LooperConfig::get_OutputChannelList() const
{
	return m_outputChannelIds;
}

size_t LooperConfig::get_OutputChannelCount() const
{
	return m_numOutputIds;
}

int LooperConfig::get_SampleCount() const
{
	return m_sampleCount;
}

void LooperConfig::put_SampleCount(int value)
{
	m_sampleCount = value;
}


float LooperConfig::get_OutputSaturation() const
{
	return m_outputSaturation;
}

void LooperConfig::put_OutputSaturation(float value)
{
	m_outputSaturation = value;
}
