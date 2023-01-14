#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ITake.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("87b0be8d-e08e-4f47-af6b-78bdac98f595")) ITimeline;

				_COM_SMARTPTR_TYPEDEF(ITimeline, __uuidof(ITimeline));

				__interface ITimeline : public IUnknown
				{
					int get_TakeCount() = 0;

					_declspec(property(get = get_TakeCount)) int TakeCount;

					ITakePtr get_Take(int id) = 0;

					_declspec(property(get = get_Take)) ITakePtr Takes[];

					int AddTake(ITakePtr take) = 0;
					bool RemoveTake(int id) = 0;
					bool MoveTake(int id, Time to) = 0;
				};
			}
		}
	}
}
