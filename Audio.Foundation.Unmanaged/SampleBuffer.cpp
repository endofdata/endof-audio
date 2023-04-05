#include "pch.h"
#include "SampleBuffer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

SampleBuffer::SampleBuffer(int sampleCount)
{
	m_pSamples = new Sample[sampleCount];
	if(nullptr == m_pSamples)
		throw new std::invalid_argument("SampleBuffer: Not enough memory for sample data buffer.");

	m_iSamples = sampleCount;
	Clear();
}

SampleBuffer::SampleBuffer(Sample* pSamples, int sampleCount)
{
	m_pSamples = pSamples;
	if (nullptr == m_pSamples)
		throw new std::invalid_argument("SampleBuffer: Data buffer cannot be null.");

	m_iSamples = sampleCount;
}

SampleBuffer::~SampleBuffer()
{
	delete[] m_pSamples;
}
