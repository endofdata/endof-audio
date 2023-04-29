#include "pch.h"
#include "Take.h"

using namespace Audio::Foundation::Unmanaged;

Take::Take(int id, ISampleContainerPtr& container, AudioTime position, AudioTime length) :
	m_id(id),
	m_container(container),
	m_position(position),
	m_length(length),
	m_refCount(0)
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


int Take::get_Id() const
{
	return m_id;
}

AudioTime Take::get_Position() const
{
	return m_position;
}

void Take::put_Position(AudioTime value)
{
	m_position = value;
}

AudioTime Take::get_Length() const
{
	return m_length;
}

void Take::put_Length(AudioTime value)
{
	m_length = value;
}

AudioTime Take::get_EndPosition() const
{
	return m_position + m_length;
}

ISampleContainerPtr Take::get_Container()
{
	return m_container;
}

MixParameter& Take::get_Mix()
{
	return m_mix;
}

bool Take::HasDataAt(AudioTime position) const
{
	return Position <= position && EndPosition > position;
}

bool Take::SeekTo(AudioTime offset, AudioSeek kind)
{
	AudioTime newPosition;

	switch (kind)
	{
	case AudioSeek::SeekAbsolute:
		newPosition = offset;
		break;
	case AudioSeek::SeekCurrent:
		newPosition = m_readOffset + offset;
		break;
	case AudioSeek::SeekEnd:
		newPosition = EndPosition + offset;
		break;
	case AudioSeek::SeekStart:
		newPosition = Position + offset;
		break;
	default:
		throw std::runtime_error("Unsupported AudioSeek value");
	}

	if (HasDataAt(newPosition))
	{
		m_readOffset = newPosition;
		return true;
	}
	return false;
}


int Take::WriteTo(ISampleContainerPtr& target, int channelOffset, int channelCount, int targetChannelOffset)
{
	int done = m_container->WriteTo(target, m_readOffset, target->SampleCount, channelOffset, channelCount, 0, targetChannelOffset, m_mix, true);
	m_readOffset += done;

	return done;
}
