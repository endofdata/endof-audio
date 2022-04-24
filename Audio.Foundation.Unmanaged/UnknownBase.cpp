#include "pch.h"

#include "UnknownBase.h"

using namespace Audio::Foundation::Unmanaged;


UnknownBase::UnknownBase() : m_refCount(0)
{
}

// virtual 
HRESULT UnknownBase::QueryInterface(REFIID riid, void** ppvObject)
{
	if (ppvObject == NULL)
	{
		return E_INVALIDARG;
	}

	*ppvObject = NULL;

	if (GetInterface(riid, ppvObject))
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

// virtual
UnknownBase::~UnknownBase()
{
}

// virtual 
ULONG UnknownBase::AddRef()
{
	::InterlockedIncrement(&m_refCount);
	return m_refCount;
}

// virtual 
ULONG UnknownBase::Release()
{
	if (::InterlockedDecrement(&m_refCount) == 0)
	{
		delete this;
		return 0;
	}
	return m_refCount;
}

// virtual 
bool UnknownBase::GetInterface(REFIID riid, void** pResult)
{
	if (riid == _uuidof(IUnknown))
	{
		*pResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	return false;
}