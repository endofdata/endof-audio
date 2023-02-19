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
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("fcc39f32-a0c4-4ece-a562-0a051c153a95")) IMeterChannel;

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IMeterChannel, __uuidof(IMeterChannel));

				typedef int(__stdcall* MeterChannelCallback)(IMeterChannelPtr pSender);


				__interface IMeterChannel : public IUnknown
				{
					int get_ChannelCount() = 0;

					_declspec(property(get = get_ChannelCount)) int ChannelCount;

					int get_RMSTime() = 0;
					void put_RMSTime(int value) = 0;

					_declspec(property(get = get_RMSTime, put = put_RMSTime)) int RMSTime;

					float get_DbFS(int index) = 0;

					_declspec(property(get = get_DbFS)) float DbFS[];

					MeterChannelCallback get_MeterUpdate() = 0;
					void put_MeterUpdate(MeterChannelCallback value) = 0;

					_declspec(property(get = get_MeterUpdate, put = put_MeterUpdate)) MeterChannelCallback MeterUpdate;

					void Reset() = 0;
				};
			}
		}
	}
}
