#include "pch.h"
#include "ProcessingContext.h"

using namespace Audio::Foundation::Unmanaged;

ProcessingContext::ProcessingContext(int sampleCount) :
	m_sampleCount(sampleCount),
	m_samplePosition(0),
	m_loopStartSample(0),
	m_loopEndSample(0),
	m_isLooping(false),
	m_isLoopStart(false),
	m_isLoopEnd(false)
{
}

ProcessingContext::~ProcessingContext()
{
}

int ProcessingContext::get_SampleCount() const
{
	return m_sampleCount;
}

int ProcessingContext::get_SamplePosition() const
{
	return m_samplePosition;
}

void ProcessingContext::put_SamplePosition(int value)
{
	if (m_isLooping)
	{
		if (m_isLoopEnd)
		{
			value = m_loopStartSample;
		}
		m_isLoopStart = value < m_loopStartSample + m_sampleCount;
		m_isLoopEnd = value >= m_loopEndSample - m_sampleCount;
	}
	m_samplePosition = value;
}

bool ProcessingContext::get_IsLoopStart() const
{
	return m_isLoopStart;
}

void ProcessingContext::put_IsLoopStart(bool value)
{
	m_isLoopStart = value;
}

bool ProcessingContext::get_IsLoopEnd() const
{
	return m_isLoopEnd;
}

void ProcessingContext::put_IsLoopEnd(bool value)
{
	m_isLoopEnd = value;
}

bool ProcessingContext::get_IsLooping()
{
	return m_isLooping;
}

void ProcessingContext::put_IsLooping(bool value)
{
	m_isLooping = value;
	SamplePosition = m_samplePosition;
	m_isLoopEnd = true;
}

int ProcessingContext::get_LoopStartSample()
{
	return m_loopStartSample;
}

void ProcessingContext::put_LoopStartSample(int value)
{
	if (value > m_loopEndSample)
	{
		m_loopStartSample = m_loopEndSample;
		m_loopEndSample = value;
	}
	else
	{
		m_loopStartSample = value;
	}
	SamplePosition = m_samplePosition;
}

int ProcessingContext::get_LoopEndSample()
{
	return m_loopEndSample;
}

void ProcessingContext::put_LoopEndSample(int value)
{
	if (value < m_loopStartSample)
	{
		m_loopEndSample = m_loopStartSample;
		m_loopStartSample = value;
	}
	else
	{
		m_loopEndSample = value;
	}
	SamplePosition = m_samplePosition;
}