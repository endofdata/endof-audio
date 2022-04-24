#include "pch.h"
#include "SampleBuffer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

SampleBuffer::SampleBuffer(int iSamples)
{
	m_pSamples = new float[iSamples];
	if(NULL == m_pSamples)
		throw new std::invalid_argument("SampleBuffer: Not enough memory for sample data buffer.");

	m_iSamples = iSamples;
	Flush();
}

SampleBuffer::~SampleBuffer()
{
	delete[] m_pSamples;
}

void SampleBuffer::Flush()
{
	ZeroMemory(m_pSamples, sizeof(float) * m_iSamples);
}

float SampleBuffer::get_Sample(int iIdx)
{
	return m_pSamples[iIdx];
}

void SampleBuffer::put_Sample(int iIdx, float value)
{
	m_pSamples[iIdx] = value;
}

int SampleBuffer::get_SampleCount()
{
	return m_iSamples;
}

float* SampleBuffer::get_SamplePtr()
{
	return m_pSamples;
}
