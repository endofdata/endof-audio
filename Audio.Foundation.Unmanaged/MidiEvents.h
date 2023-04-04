#pragma once
#include <MidiMessage.h>
#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "IMidiEvents.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class MidiEvents : public IMidiEvents
			{
			public:
				MidiEvents();
				virtual ~MidiEvents();

				void Open();
				void Close();
				void Error(const MidiMessage& msg, unsigned int timeStamp);
				void Overflow(const MidiMessage& msg, unsigned int timeStamp);
				void Data(const MidiMessage& msg, unsigned int timeStamp);

				void* get_Context();
				void put_Context(void* value);

				EventHandler get_OnOpen();
				void put_OnOpen(EventHandler value);

				EventHandler get_OnClose();
				void put_OnClose(EventHandler value);

				MessageHandler get_OnError();
				void put_OnError(MessageHandler value);

				MessageHandler get_OnOverflow();
				void put_OnOverflow(MessageHandler value);

				MessageHandler get_OnData();
				void put_OnData(MessageHandler value);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				void Invoke(EventHandler handler);
				void Invoke(MessageHandler handler, const MidiMessage& msg, unsigned int timeStamp);

				void* m_pContext;

				EventHandler m_onOpen;
				EventHandler m_onClose;
				MessageHandler m_onError;
				MessageHandler m_onOverflow;
				MessageHandler m_onData;

			};
		}
	}
}
