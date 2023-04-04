#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <AudioTime.h>
#include <comdef.h>
#include <MidiMessage.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				typedef void (*EventHandler)(void* pContext);
				typedef void (*MessageHandler)(void* pContext, const MidiMessage& msg, unsigned int timeStamp);

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("b35a45ad-4bce-4892-944d-fbe77f3a4a02")) IMidiEvents : public IUnknown
				{
					void Open() = 0;
					void Close() = 0;
					void Error(const MidiMessage& msg, unsigned int timeStamp) = 0;
					void Overflow(const MidiMessage& msg, unsigned int timeStamp) = 0;
					void Data(const MidiMessage& msg, unsigned int timeStamp) = 0;

					void* get_Context() = 0;
					void put_Context(void* value) = 0;

					_declspec(property(get = get_Context, put = put_Context)) void* Context;

					EventHandler get_OnOpen() = 0;
					void put_OnOpen(EventHandler value) = 0;

					_declspec(property(get = get_OnOpen, put = put_OnOpen)) EventHandler OnOpen;

					EventHandler get_OnClose() = 0;
					void put_OnClose(EventHandler value) = 0;

					_declspec(property(get = get_OnClose, put = put_OnClose)) EventHandler OnClose;

					MessageHandler get_OnError() = 0;
					void put_OnError(MessageHandler value) = 0;

					_declspec(property(get = get_OnError, put = put_OnError)) MessageHandler OnError;

					MessageHandler get_OnOverflow() = 0;
					void put_OnOverflow(MessageHandler value) = 0;

					_declspec(property(get = get_OnOverflow, put = put_OnOverflow)) MessageHandler OnOverflow;

					MessageHandler get_OnData() = 0;
					void put_OnData(MessageHandler value) = 0;

					_declspec(property(get = get_OnData, put = put_OnData)) MessageHandler OnData;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IMidiEvents, __uuidof(IMidiEvents));
			}
		}
	}
}