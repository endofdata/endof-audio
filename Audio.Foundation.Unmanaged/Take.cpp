#include "pch.h"
#include "Take.h"

using namespace Audio::Foundation::Unmanaged;

Take::Take(int id, ISampleContainerPtr container, Time position, Time length) :
	m_id(id),
	m_container(container),
	m_position(position),
	m_length(length)
{
}

Take::~Take()
{
}

IMPLEMENT_IUNKNOWN(Take)

bool Take::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ITake*>(this));
		return true;
	}
	if (iid == __uuidof(ITake))
	{
		*ppvResult = dynamic_cast<ITake*>(this);
		return true;
	}
	return false;
}


int Take::get_Id()
{
	return m_id;
}

Time Take::get_Position()
{
	return m_position;
}

void Take::put_Position(Time value)
{
	m_position = value;
}

Time Take::get_Length()
{
	return m_length;
}

void Take::put_Length(Time value)
{
	m_length = value;
}

ISampleContainerPtr Take::get_Container()
{
	return m_container;
}



