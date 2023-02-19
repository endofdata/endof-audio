#include "pch.h"
#include "TakeSequence.h"
#include <algorithm>
#include "ObjectFactory.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;



TakeSequence::TakeSequence() :
	m_isActive(false),
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
	Time position = take->Position;

	m_takes.insert(std::find_if(m_takes.begin(), m_takes.end(), [position](ITakePtr t) { return t->Position >= position;  }), take);
	m_playPosition = m_takes.begin();

	return take->Id;
}

bool TakeSequence::RemoveTake(int takeId)
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

bool TakeSequence::MoveTake(int takeId, Time to)
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
	auto matches = std::find_if(m_takes.begin(), m_takes.end(), [takeId](ITakePtr t) { return t->Id == takeId;  });

	if (matches == m_takes.end())
	{
		return nullptr;
	}
	return matches[0];
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
	if (HasNext)
	{
		m_pNext->Process(container);
	}
}
