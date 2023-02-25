#include "pch.h"
#include "TakeSequence.h"
#include <algorithm>
#include "ObjectFactory.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;



TakeSequence::TakeSequence(IHostClockPtr hostClock, ISampleContainerPtr targetContainer) :
	m_pHostClock(hostClock),
	m_currentTime(0),
	m_playPosition(m_takes.end()),
	m_scheduledTake(m_takes.end()),
	m_scheduledTime(0),
	m_pTargetContainer(targetContainer),
	m_refCount(0)
{
}

TakeSequence::~TakeSequence()
{
}

IMPLEMENT_IUNKNOWN(TakeSequence)

bool TakeSequence::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ITakeSequence*>(this));
		return true;
	}
	if (iid == __uuidof(ITakeSequence))
	{
		*ppvResult = dynamic_cast<ITakeSequence*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}

int TakeSequence::get_TakeCount()
{
	return (int)m_takes.size();
}


ITakePtr TakeSequence::get_Take(int index)
{
	if (index < 0 || index >= m_takes.size())
	{
		return nullptr;
	}
	return m_takes[index];
}

int TakeSequence::AddTake(ITakePtr take)
{
	AudioTime position = take->Position;

	m_takes.insert(std::find_if(m_takes.begin(), m_takes.end(), [position](ITakePtr t) { return t->Position >= position;  }), take);
	m_scheduledTake = m_takes.begin();
	put_PlayPosition(m_pHostClock->CurrentTime);

	return take->Id;
}

bool TakeSequence::RemoveTake(int takeId)
{
	auto takeToRemove = FindTake(takeId);

	if(takeToRemove != nullptr)
	{
		auto result = std::remove(m_takes.begin(), m_takes.end(), takeToRemove);
		m_takes.erase(result, m_takes.end());
		m_scheduledTake = m_takes.begin();
		put_PlayPosition(m_pHostClock->CurrentTime);

		return true;
	}
	return false;
}

bool TakeSequence::MoveTake(int takeId, AudioTime to)
{
	auto takeToMove = FindTake(takeId);

	if (takeToMove != nullptr)
	{
		auto result = std::remove(m_takes.begin(), m_takes.end(), takeToMove);
		m_takes.erase(result, m_takes.end());
		takeToMove->Position = to;
		AddTake(takeToMove);

		return true;
	}
	return false;
}

ITakePtr TakeSequence::FindTake(int takeId)
{
	auto matches = std::find_if(m_takes.begin(), m_takes.end(), [takeId](const ITakePtr& t) { return t->Id == takeId;  });

	if (matches == m_takes.end())
	{
		return nullptr;
	}
	return matches[0];
}

AudioTime TakeSequence::get_PlayPosition()
{
	return m_currentTime;
}

void TakeSequence::put_PlayPosition(AudioTime currentTime)
{
	if(m_scheduledTake != m_takes.end())
	{
		if (!(*m_scheduledTake)->HasDataAt(currentTime))
		{
			if (currentTime < m_currentTime)
			{
				m_scheduledTake = m_takes.begin();
			}
			m_scheduledTake = std::find_if(m_scheduledTake, m_takes.end(), [currentTime](const ITakePtr& t) { return t->EndPosition < currentTime; });

			if (m_scheduledTake != m_takes.end())
			{
				m_scheduledTime = (*m_scheduledTake)->Position;
			}
		}
	}
	m_playPosition = m_takes.end();
	m_currentTime = currentTime;
}

ISampleProcessorPtr TakeSequence::get_Next()
{
	return m_pNext;
}

void TakeSequence::put_Next(ISampleProcessorPtr value)
{
	m_pNext = value;
}

bool TakeSequence::get_HasNext()
{
	return m_pNext != nullptr;
}

void TakeSequence::Process(ISampleContainerPtr container)
{	
	bool hasTake = false;

	for (int channelOffset = 0; channelOffset < m_pTargetContainer->ChannelCount; channelOffset += container->ChannelCount)
	{
		container->CopyTo(m_pTargetContainer, 0, container->SampleCount, 0, container->ChannelCount, 0, channelOffset);
	}

	if (m_scheduledTake != m_takes.end())
	{
		m_currentTime = m_pHostClock->CurrentTime;

		if (m_currentTime >= m_scheduledTime)
		{
			m_playPosition = m_scheduledTake;
			m_scheduledTake = m_takes.end();
			hasTake = true;
		}
	}
	else
	{
		hasTake = m_playPosition != m_takes.end();
	}
	if (hasTake)
	{
		int done = (*m_playPosition)->ReadSamplesTo(m_pTargetContainer, 0, m_pTargetContainer->ChannelCount, 0);

		if (!done)
		{
			m_scheduledTake = m_playPosition + 1;
			if (m_scheduledTake != m_takes.end())
			{
				m_scheduledTime = (*m_scheduledTake)->Position;
			}
			m_playPosition = m_takes.end();
		}
	}
	if (HasNext)
	{
		m_pNext->Process(m_pTargetContainer);
	}
}
