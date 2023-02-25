#include "pch.h"
#include "SampleBufferSpan.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;

SampleBufferSpan::SampleBufferSpan() :
	m_pSamples(nullptr),
	m_iSamples(0),
	m_refCount(0)
{
}

SampleBufferSpan::SampleBufferSpan(Sample* pSamples, int sampleCount) : 
	m_pSamples(pSamples),
	m_iSamples(sampleCount),
	m_refCount(0)
{
}

SampleBufferSpan::~SampleBufferSpan()
{
}

IMPLEMENT_IUNKNOWN(SampleBufferSpan)

bool SampleBufferSpan::GetInterface(REFIID iid, void** ppvResult)
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

void SampleBufferSpan::Clear()
{
	ZeroMemory(m_pSamples, sizeof(Sample) * m_iSamples);
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
	return m_iSamples;
}

Sample* SampleBufferSpan::get_SamplePtr()
{
	return m_pSamples;
}

int SampleBufferSpan::AddTo(ISampleBufferPtr other, int sampleOffset, int sampleCount, int targetOffset) const
{
	int sourceMax = GetEffectiveCount(sampleOffset, sampleCount, SampleCount);
	int targetMax = GetEffectiveCount(targetOffset, sampleCount, other->SampleCount);
	int count = std::min(sourceMax, targetMax);

	if (count > 0)
	{
		const Sample* pSource = &m_pSamples[sampleOffset];
		Sample* pTarget = &other->SamplePtr[targetOffset];

		for (int s = 0; s < count; s++)
		{
			*pTarget++ += *pSource++;
		}
	}
	return count;
}

int SampleBufferSpan::CopyTo(ISampleBufferPtr other, int sampleOffset, int sampleCount, int targetOffset) const
{
	int sourceMax = GetEffectiveCount(sampleOffset, sampleCount, SampleCount);
	int targetMax = GetEffectiveCount(targetOffset, sampleCount, other->SampleCount);
	int count = std::min(sourceMax, targetMax);

	if (count > 0)
	{
		const Sample* pSource = &m_pSamples[sampleOffset];
		Sample* pTarget = &other->SamplePtr[targetOffset];

		std::memcpy(pTarget, pSource, count * sizeof(Sample));
	}
	return count;
}


int SampleBufferSpan::GetEffectiveCount(int requestedOffset, int requestedCount, int maxCount) const
{
	return std::max(0, std::min(requestedCount, maxCount - requestedOffset));
}