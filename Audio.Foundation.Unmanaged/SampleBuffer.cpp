#include "pch.h"
#include "SampleBuffer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

SampleBuffer::SampleBuffer(int iSamples) : m_refCount(0)
{
	m_pSamples = new Sample[iSamples];
	if(nullptr == m_pSamples)
		throw new std::invalid_argument("SampleBuffer: Not enough memory for sample data buffer.");

	m_iSamples = iSamples;
	Clear();
}

SampleBuffer::~SampleBuffer()
{
	delete[] m_pSamples;
}

IMPLEMENT_IUNKNOWN(SampleBuffer)

bool SampleBuffer::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleBuffer))
	{
		*ppvResult = dynamic_cast<ISampleBuffer*>(this);
		return true;
	}
	return false;

}

void SampleBuffer::Clear()
{
	ZeroMemory(m_pSamples, sizeof(Sample) * m_iSamples);
}

Sample SampleBuffer::get_Sample(int iIdx)
{
	return m_pSamples[iIdx];
}

void SampleBuffer::put_Sample(int iIdx, Sample value)
{
	m_pSamples[iIdx] = value;
}

int SampleBuffer::get_SampleCount()
{
	return m_iSamples;
}

Sample* SampleBuffer::get_SamplePtr()
{
	return m_pSamples;
}
