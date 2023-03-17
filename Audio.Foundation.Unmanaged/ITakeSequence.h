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
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("87b0be8d-e08e-4f47-af6b-78bdac98f595")) ITakeSequence;

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ITakeSequence, __uuidof(ITakeSequence));

				__interface ITakeSequence : public IUnknown
				{
					int get_TakeCount() = 0;

					_declspec(property(get = get_TakeCount)) int TakeCount;


					ITakePtr get_Take(int index) = 0;

					_declspec(property(get = get_Take)) ITakePtr Takes[];

					AudioTime get_PlayPosition() = 0;
					void put_PlayPosition(AudioTime value) = 0;

					_declspec(property(get = get_PlayPosition, put = put_PlayPosition)) AudioTime PlayPosition;


					int AddTake(ITakePtr& take) = 0;
					bool RemoveTake(int takeId) = 0;
					bool MoveTake(int takeId, AudioTime to) = 0;
					ITakePtr FindTake(int takeId) = 0;
				};
			}
		}
	}
}
