#pragma once
#include <comdef.h>

#define DECLARE_IUNKNOWN															\
	public:																			\
		virtual STDMETHODIMP QueryInterface(REFIID riid, void **ppv);				\
		virtual STDMETHODIMP_(ULONG) AddRef();										\
		virtual STDMETHODIMP_(ULONG) Release();										\
	private:																		\
		ULONG m_refCount;															\

#define IMPLEMENT_IUNKNOWN(TYPE_NAME)												\
	HRESULT TYPE_NAME::QueryInterface(const IID& riid, void** ppvObject)			\
	{																				\
		if (ppvObject == NULL)														\
		{																			\
			return E_INVALIDARG;													\
		}																			\
																					\
		*ppvObject = NULL;															\
																					\
		if (GetInterface(riid, ppvObject))											\
		{																			\
			AddRef();																\
			return S_OK;															\
		}																			\
		return E_NOINTERFACE;														\
	}																				\
																					\
	ULONG STDMETHODCALLTYPE TYPE_NAME::AddRef(void)									\
	{																				\
		::InterlockedIncrement(&m_refCount);										\
		return m_refCount;															\
	}																				\
																					\
	ULONG STDMETHODCALLTYPE TYPE_NAME::Release(void)								\
	{																				\
		if (::InterlockedDecrement(&m_refCount) == 0)								\
		{																			\
			delete this;															\
			return 0;																\
		}																			\
		return m_refCount;															\
	}																			
																																						