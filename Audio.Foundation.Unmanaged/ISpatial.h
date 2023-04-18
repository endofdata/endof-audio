#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <AudioTime.h>
#include <comdef.h>

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
					double get_Level() const = 0;
					void put_Level(double value) = 0;
					_declspec(property(get = get_Level, put = put_Level)) double Level;

					double get_Pan() const = 0;
					void put_Pan(double value) = 0;
					_declspec(property(get = get_Pan, put = put_Pan)) double Pan;

				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISpatial, __uuidof(ISpatial));
			}
		}
	}
}