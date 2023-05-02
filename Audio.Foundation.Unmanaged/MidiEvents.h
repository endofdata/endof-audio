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

				MidiEventHandler get_OnOpen();
				void put_OnOpen(MidiEventHandler value);

				MidiEventHandler get_OnClose();
				void put_OnClose(MidiEventHandler value);

				MidiMessageHandler get_OnError();
				void put_OnError(MidiMessageHandler value);

				MidiMessageHandler get_OnOverflow();
				void put_OnOverflow(MidiMessageHandler value);

				MidiMessageHandler get_OnData();
				void put_OnData(MidiMessageHandler value);

				DECLARE_IUNKNOWN

			private:
				void Invoke(MidiEventHandler handler);
				void Invoke(MidiMessageHandler handler, const MidiMessage& msg, unsigned int timeStamp);

				void* m_pContext;

				MidiEventHandler m_onOpen;
				MidiEventHandler m_onClose;
				MidiMessageHandler m_onError;
				MidiMessageHandler m_onOverflow;
				MidiMessageHandler m_onData;

			};
		}
	}
}
