#include "pch.h"
#include "SampleJoiner.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleJoiner::SampleJoiner() :
	m_pNext(nullptr),
	m_refCount(0)
{
}

SampleJoiner::~SampleJoiner()
{
}

IMPLEMENT_IUNKNOWN(SampleJoiner)

bool SampleJoiner::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleJoiner*>(this));
		return true;
	}
	if (iid == __uuidof(ISampleJoiner))
	{
		*ppvResult = dynamic_cast<ISampleJoiner*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}

void SampleJoiner::AddSource(ISampleContainerPtr Source)
{
	Source.AddRef();

	// do not allow two sends to the same destination
	RemoveSource(Source);

	m_vecSources.push_back(Source);
}

void SampleJoiner::RemoveSource(ISampleContainerPtr Source)
{
	std::vector<ISampleContainerPtr>::iterator newEnd =
		remove_if(m_vecSources.begin(), m_vecSources.end(), [&Source](ISampleContainerPtr item)
	{
		if (item == Source)
		{
			return true;
		}
		return false;
	});
	m_vecSources.erase(newEnd, m_vecSources.end());
}

void SampleJoiner::RemoveAllSources()
{
	m_vecSources.clear();
}

ISampleContainerPtr SampleJoiner::get_Source(int index)
{
	return m_vecSources.at(index);
}

ISampleProcessorPtr SampleJoiner::get_Next()
{
	return m_pNext;
}

void SampleJoiner::put_Next(ISampleProcessorPtr value)
{
	m_pNext = value;
}

bool SampleJoiner::get_HasNext()
{
	return m_pNext != nullptr;
}

void SampleJoiner::Process(ISampleContainerPtr container)
{
	if (HasNext)
	{
		int targetChannelCount = container->ChannelCount;

		if (targetChannelCount > 0)
		{
			int targetSampleCount = container->Channels[0]->SampleCount;

			std::for_each(m_vecSources.begin(), m_vecSources.end(), [this, &container, targetChannelCount, targetSampleCount](ISampleContainerPtr item)
			{
				int maxChannels = std::min(targetChannelCount, item->ChannelCount);

				for (int c = 0; c < maxChannels; c++)
				{
					ISampleBufferPtr channel = item->Channels[c];

					int maxSamples = std::min(targetSampleCount, channel->SampleCount);

					const Sample* pSource = channel->SamplePtr;
					Sample* pTarget = container->Channels[c]->SamplePtr;

					for (int s = 0; s < maxSamples; s++)
					{
						*pTarget++ += *pSource++;
					}
				}
			});
		}
		m_pNext->Process(container);
	}
}
