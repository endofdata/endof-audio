#include "pch.h"
#include "Timeline.h"
#include <algorithm>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;



Timeline::Timeline()
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
	return false;
}

int Timeline::get_TakeCount()
{
	return (int)m_takes.size();
}


ITakePtr Timeline::get_Take(int id)
{
	return m_takes[id];
}

int Timeline::AddTake(ITakePtr take)
{
	Time position = take->Position;

	m_takes.insert(std::find_if(m_takes.begin(), m_takes.end(), [position](ITakePtr t) { return t->Position >= position;  }), take);

	return take->Id;
}

bool Timeline::RemoveTake(int id)
{
	auto takeToRemove = GetTakeById(id);

	if(takeToRemove != NULL)
	{
		std::remove(m_takes.begin(), m_takes.end(), takeToRemove);

		return true;
	}
	return false;
}

bool Timeline::MoveTake(int id, Time to)
{
	auto takeToMove = GetTakeById(id);

	if (takeToMove != NULL)
	{
		std::remove(m_takes.begin(), m_takes.end(), takeToMove);
		AddTake(takeToMove);

		return true;
	}
	return false;
}

ITakePtr Timeline::GetTakeById(int id)
{
	auto matches = std::find_if(m_takes.begin(), m_takes.end(), [id](ITakePtr t) { return t->Id == id;  });

	if (matches == m_takes.end())
	{
		return NULL;
	}
	return matches[0];
}
