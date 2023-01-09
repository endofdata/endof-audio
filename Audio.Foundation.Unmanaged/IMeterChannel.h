#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface IMeterChannel;
				__interface ISampleReceiver;

				typedef int(__stdcall* MeterChannelCallback)(IMeterChannel* pSender);

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("fcc39f32-a0c4-4ece-a562-0a051c153a95")) IMeterChannel : public IUnknown
				{
					int get_RMSTime() = 0;
					void put_RMSTime(int value) = 0;

					_declspec(property(get = get_RMSTime, put = put_RMSTime)) int RMSTime;

					float get_DbFS(int index) = 0;

					_declspec(property(get = get_DbFS)) float DbFS[];

					MeterChannelCallback get_MeterUpdate() = 0;
					void put_MeterUpdate(MeterChannelCallback value) = 0;

					_declspec(property(get = get_MeterUpdate, put = put_MeterUpdate)) MeterChannelCallback MeterUpdate;

					ISampleReceiver* get_WriteThrough() = 0;
					void put_WriteThrough(ISampleReceiver* value) = 0;

					_declspec(property(get = get_WriteThrough, put = put_WriteThrough)) ISampleReceiver* WriteThrough;
				};
			}
		}
	}
}
