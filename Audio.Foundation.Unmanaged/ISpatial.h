#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <MixParameter.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				/// <summary>
				/// Level / pan control
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("6357e908-9ec1-48d6-91f2-4f85c261f2bd")) ISpatial : public IUnknown
				{
					MixParameter& get_Mix();
					_declspec(property(get = get_Mix)) MixParameter& Mix;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISpatial, __uuidof(ISpatial));
			}
		}
	}
}