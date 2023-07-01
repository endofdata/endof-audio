#pragma once

#include <Audio.Foundation.Unmanaged.h>
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
				enum class _AUDIO_FOUNDATION_UNMANAGED_API ControllerCode
				{
					None,
					Stop,
					Run,
					Locate,
					RecordArm,
					Record					
				};

				typedef void (*ControllerEventHandler)(void* pContext, ControllerCode code);

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("c192ed5f-7216-4f8e-9348-9376b3d02951")) IControllerEvents : public IUnknown
				{
					/// <summary>
					/// Triggers the <see cref="OnCommand"/> event
					/// </summary>
					void ControllerCommand(ControllerCode code) = 0;

					void* get_Context() = 0;
					void put_Context(void* value) = 0;
					/// <summary>
					/// Gets or sets the context parameter for the transport event handler
					/// </summary>
					_declspec(property(get = get_Context, put = put_Context)) void* Context;


					ControllerEventHandler get_OnCommand() = 0;
					void put_OnCommand(ControllerEventHandler value) = 0;
					/// <summary>
					/// Gets or sets the controller command event handler
					/// </summary>
					_declspec(property(get = get_OnCommand, put = put_OnCommand)) ControllerEventHandler OnCommand;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IControllerEvents, __uuidof(IControllerEvents));
			}
		}
	}
}