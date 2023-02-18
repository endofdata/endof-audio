#include "pch.h"
#include "Timeline.h"
#include <algorithm>
#include "ObjectFactory.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;



Timeline::Timeline() :
	m_isActive(false),
	m_refCount(0)
{
}

Timeline::~Timeline()
{
}

IMPLEMENT_IUNKNOWN(Timeline)

bool Timeline::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ITimeline*>(this));
		return true;
	}
	if (iid == __uuidof(ITimeline))
	{
		*ppvResult = dynamic_cast<ITimeline*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}

int Timeline::get_TakeCount()
{
	return (int)m_takes.size();
}


ITakePtr Timeline::get_Take(int index)
{
	if (index < 0 || index >= m_takes.size())
	{
		return nullptr;
	}
	return m_takes[index];
}

int Timeline::AddTake(ITakePtr take)
{
	Time position = take->Position;

	m_takes.insert(std::find_if(m_takes.begin(), m_takes.end(), [position](ITakePtr t) { return t->Position >= position;  }), take);
	m_playPosition = m_takes.begin();

	return take->Id;
}

bool Timeline::RemoveTake(int takeId)
{
	auto takeToRemove = FindTake(takeId);

	if(takeToRemove != nullptr)
	{
		auto result = std::remove(m_takes.begin(), m_takes.end(), takeToRemove);
		m_takes.erase(result, m_takes.end());
		m_playPosition = m_takes.begin();

		return true;
	}
	return false;
}

bool Timeline::MoveTake(int takeId, Time to)
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

ITakePtr Timeline::FindTake(int takeId)
{
	auto matches = std::find_if(m_takes.begin(), m_takes.end(), [takeId](ITakePtr t) { return t->Id == takeId;  });

	if (matches == m_takes.end())
	{
		return nullptr;
	}
	return matches[0];
}


ISampleProcessorPtr Timeline::get_Next()
{
	return m_pNext;
}

void Timeline::put_Next(ISampleProcessorPtr value)
{
	m_pNext = value;
}

bool Timeline::get_HasNext()
{
	return m_pNext != nullptr;
}

void Timeline::Process(ISampleContainerPtr container)
{
	if (HasNext)
	{
		m_pNext->Process(container);
	}
}
