#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ITransportEvents.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API _declspec(uuid("c01bbd40-980c-4a1b-bdee-6b213798ff99")) ITransportControl : public IUnknown
				{
					bool get_IsActive() const = 0;
					void put_IsActive(bool value) = 0;
					_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

					bool GetNext(unsigned int timeout, TransportCode& code) = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ITransportControl, __uuidof(ITransportControl));
			}
		}
	}
}
