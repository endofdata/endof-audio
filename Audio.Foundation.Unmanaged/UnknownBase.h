#pragma once
#include <comdef.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class UnknownBase : public IUnknown
			{
			public:
				UnknownBase();
				virtual ~UnknownBase();

				virtual HRESULT STDMETHODCALLTYPE QueryInterface(
					/* [in] */ REFIID riid,
					/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject);

				virtual ULONG STDMETHODCALLTYPE AddRef(void);

				virtual ULONG STDMETHODCALLTYPE Release(void);

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ULONG m_refCount;
			};
		}
	}
}