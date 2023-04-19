#include "pch.h"
#include "VstHostApplication.h"
#include <algorithm>

using namespace Audio::Vst::Unmanaged;
using namespace Steinberg;
using namespace Steinberg::Vst;

VstHostApplication::VstHostApplication(const std::wstring& name) :
	m_name(name),
	m_refCount(0)
{
}

VstHostApplication::~VstHostApplication()
{

}

tresult VstHostApplication::queryInterface(const TUID iid, void** obj)
{
	if (FUnknownPrivate::iidEqual(iid, FUnknown_iid))
	{
		*obj = reinterpret_cast<FUnknown*>(this);
		addRef();
		return kResultOk;
	}
	if (FUnknownPrivate::iidEqual(iid, IHostApplication_iid))
	{
		*obj = reinterpret_cast<IHostApplication*>(this);
		addRef();
		return kResultOk;
	}
	if (FUnknownPrivate::iidEqual(iid, IPlugInterfaceSupport_iid))
	{
		*obj = reinterpret_cast<IPlugInterfaceSupport*>(this);
		addRef();
		return kResultOk;
	}
	*obj = nullptr;

	return kNoInterface;
}

uint32 VstHostApplication::addRef()
{
	return InterlockedIncrement(&m_refCount);
}

uint32 VstHostApplication::release()
{
	uint32 result = InterlockedDecrement(&m_refCount);

	if (result == 0)
	{
		delete this;
	}
	return result;
}

tresult VstHostApplication::getName(String128 name)
{
	size_t count = std::min(m_name.length(), static_cast<size_t>(127));
	m_name.copy(name, count);

	return static_cast<tresult>(count);
}

tresult VstHostApplication::createInstance(TUID cid, TUID iid, void** obj)
{
	if (FUnknownPrivate::iidEqual(iid, IMessage_iid))
	{
		// TODO: This might be worth to be implemented
		return kNoInterface;
	}
	return kNoInterface;
}

tresult VstHostApplication::isPlugInterfaceSupported(const TUID iid)
{
	return kResultFalse;
}