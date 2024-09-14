#include "pch.h"
#include "SampleBufferSpan.h"
#include "MixParameter.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

SampleBufferSpan::SampleBufferSpan() :
	m_pSamples(nullptr),
	m_samples(0),
	m_pOwnedBuffer(nullptr),
	m_samplesMax(0),
	m_refCount(0)
{
}

SampleBufferSpan::SampleBufferSpan(Sample* pSamples, int sampleCount) :
	m_pSamples(pSamples),
	m_samples(sampleCount),
	m_pOwnedBuffer(nullptr),
	m_samplesMax(0),
	m_refCount(0)
{
}

SampleBufferSpan::SampleBufferSpan(Sample* pOwnedBuffer, int samplesMax, int offset, int sampleCount) :
	m_pSamples(&pOwnedBuffer[offset]),
	m_samples(sampleCount),
	m_pOwnedBuffer(pOwnedBuffer),
	m_samplesMax(samplesMax),
	m_refCount(0)
{
}

SampleBufferSpan::~SampleBufferSpan()
{
	if (m_pOwnedBuffer != nullptr)
	{
		delete[] m_pOwnedBuffer;
		m_pOwnedBuffer = nullptr;
	}
}

IMPLEMENT_IUNKNOWN(SampleBufferSpan)

void* SampleBufferSpan::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(ISampleBuffer))
	{
		return dynamic_cast<ISampleBuffer*>(this);
	}
	return nullptr;
}

void SampleBufferSpan::Clear()
{
	ZeroMemory(m_pSamples, sizeof(Sample) * m_samples);
}

Sample SampleBufferSpan::get_Sample(int iIdx) const
{
	return m_pSamples[iIdx];
}

void SampleBufferSpan::put_Sample(int iIdx, Sample value)
{
	m_pSamples[iIdx] = value;
}

int SampleBufferSpan::get_SampleCount() const
{
	return m_samples;
}

Sample* SampleBufferSpan::get_SamplePtr()
{
	return m_pSamples;
}

int SampleBufferSpan::WriteTo(ISampleBufferPtr& other, int sampleOffset, int sampleCount, int targetOffset, double level, bool overdub) const
{
	int sourceMax = GetEffectiveCount(sampleOffset, sampleCount, SampleCount);
	int targetMax = GetEffectiveCount(targetOffset, sampleCount, other->SampleCount);
	int count = std::min(sourceMax, targetMax);

	if (count > 0)
	{
		const Sample* pSource = &m_pSamples[sampleOffset];
		Sample* pTarget = &other->SamplePtr[targetOffset];

		if (overdub)
		{
			for (int s = 0; s < count; s++)
			{
				*pTarget++ += static_cast<Sample>(*pSource++ * level);
			}
		}
		else
		{
			for (int s = 0; s < count; s++)
			{
				*pTarget++ = static_cast<Sample>(*pSource++ * level);
			}
		}
	}
	return count;
}

int SampleBufferSpan::GetEffectiveCount(int requestedOffset, int requestedCount, int maxCount) const
{
	return std::max(0, std::min(requestedCount, maxCount - requestedOffset));
}