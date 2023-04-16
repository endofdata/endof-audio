#include "pch.h"
#include "ParamValueQueue.h"

using namespace Audio::Vst::Unmanaged;

ParamValueQueue::ParamValueQueue(const ParamID& id)
	: m_id(id),
	m_refCount(0)
{
}

ParamValueQueue::~ParamValueQueue()
{
}


tresult ParamValueQueue::queryInterface(const TUID _iid, void** obj)
{
	if (_iid == FUnknown_iid)
	{
		*obj = reinterpret_cast<FUnknown*>(this);
		addRef();
		return kResultOk;
	}
	if (_iid == IHostApplication_iid)
	{
		*obj = reinterpret_cast<IParamValueQueue*>(this);
		addRef();
		return kResultOk;
	}
	*obj = nullptr;

	return kNoInterface;
}

uint32 ParamValueQueue::addRef()
{
	return InterlockedIncrement(&m_refCount);
}

uint32 ParamValueQueue::release()
{
	uint32 result = InterlockedDecrement(&m_refCount);

	if (result == 0)
	{
		delete this;
	}
	return result;
}

ParamID ParamValueQueue::getParameterId()
{
	return m_id;
}

int32 ParamValueQueue::getPointCount()
{
	return m_points.size();
}

tresult ParamValueQueue::getPoint(int32 index, int32& sampleOffset, ParamValue& value)
{
	if (index < 0 || index >= m_points.size())
	{
		value = 0.0;
		sampleOffset = 0;
		return kInvalidArgument;
	}

	auto pair = m_points[index];
	sampleOffset = pair.first;
	value = pair.second;

	return kResultOk;
}

tresult ParamValueQueue::addPoint(int32 sampleOffset, ParamValue value, int32& index)
{
	index = m_points.size();
	m_points.push_back(std::pair<int32, ParamValue>(sampleOffset, value));

	return kResultOk;
}
