#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface ISampleContainer;
				__interface ISampleReceiver;

				__interface IChannelLink
				{
					_declspec(property(get = get_Volume, put = put_Volume)) float Volume;

					float get_Volume() = 0;
					void put_Volume(float value) = 0;

					_declspec(property(get = get_Pan, put = put_Pan)) float Pan;

					float get_Pan() = 0;
					void put_Pan(float value) = 0;

					_declspec(property(get = get_Input, put = put_Input)) ISampleContainer* Input;

					ISampleContainer* get_Input() = 0;
					void put_Input(ISampleContainer* input) = 0;

					_declspec(property(get = get_Output, put = put_Output)) ISampleReceiver* Output;

					ISampleReceiver* get_Output() = 0;
					void put_Output(ISampleReceiver* Output) = 0;
				};
			}
		}
	}
}
