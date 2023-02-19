#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				typedef long long int Time;

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("933cf1fb-3110-449a-b22f-69c0b5c6e090")) ITake;

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ITake, __uuidof(ITake));

				__interface ITake : public IUnknown
				{
					int get_Id() = 0;

					_declspec(property(get = get_Id, put = put_Id)) int Id;

					Time get_Position() = 0;
					void put_Position(Time value);

					_declspec(property(get = get_Position, put = put_Position)) Time Position;

					Time get_Length() = 0;
					void put_Length(Time value);

					_declspec(property(get = get_Length, put = put_Length)) Time Length;

					ISampleContainerPtr get_Container() = 0;

					_declspec(property(get = get_Container)) ISampleContainerPtr Container;
				};
			}
		}
	}
}
