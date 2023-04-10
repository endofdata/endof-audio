#include "pch.h"
#include "ProcessingContext.h"

using namespace Audio::Foundation::Unmanaged;

ProcessingContext::ProcessingContext() :
	m_samplePosition(0),
	m_timePosition(0),
	m_isSkipping(false)
{
}

ProcessingContext::ProcessingContext(int samplePosition, AudioTime timePosition, bool isSkipping) :
	m_samplePosition(samplePosition),
	m_timePosition(timePosition),
	m_isSkipping(isSkipping)
{
}

ProcessingContext::~ProcessingContext()
{
}

int ProcessingContext::get_SamplePosition() const
{
	return m_samplePosition;
}

void ProcessingContext::put_SamplePosition(int value)
{
	m_samplePosition = value;
}

AudioTime ProcessingContext::get_TimePosition() const
{
	return m_timePosition;
}

void ProcessingContext::put_TimePosition(AudioTime value)
{
	m_timePosition = value;
}

bool ProcessingContext::get_IsSkipping() const
{
	return m_isSkipping;
}

void ProcessingContext::put_IsSkipping(bool value)
{
	m_isSkipping = value;
}
