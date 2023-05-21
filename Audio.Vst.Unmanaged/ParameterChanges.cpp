#include "pch.h"
#include "ParameterChanges.h"
#include "ParamValueQueue.h"

using namespace Audio::Vst::Unmanaged;

ParameterChanges::ParameterChanges() :
	m_refCount(0)
{
}

ParameterChanges::~ParameterChanges()
{
}


tresult ParameterChanges::queryInterface(const TUID _iid, void** obj)
{
	if (_iid == FUnknown_iid)
	{
		*obj = reinterpret_cast<FUnknown*>(this);
		addRef();
		return kResultOk;
	}
	if (_iid == IHostApplication_iid)
	{
		*obj = reinterpret_cast<IParameterChanges*>(this);
		addRef();
		return kResultOk;
	}
	*obj = nullptr;

	return kNoInterface;
}

uint32 ParameterChanges::addRef()
{
	return InterlockedIncrement(&m_refCount);
}

uint32 ParameterChanges::release()
{
	uint32 result = InterlockedDecrement(&m_refCount);

	if (result == 0)
	{
		delete this;
	}
	return result;
}


int32 ParameterChanges::getParameterCount()
{
	return static_cast<int32>(m_parameters.size());
}

IParamValueQueue* ParameterChanges::getParameterData(int32 index)
{
	if (index < 0 || index >= m_parameters.size())
	{
		return nullptr;
	}
	return m_parameters[index];
}

IParamValueQueue* ParameterChanges::addParameterData(const ParamID& id, int32& index /*out*/)
{
	index = static_cast<int32>(m_parameters.size());
	IParamValueQueuePtr value = IPtr<IParamValueQueue>(new ParamValueQueue(id));
	m_parameters.push_back(value);

	return value.take();
}
