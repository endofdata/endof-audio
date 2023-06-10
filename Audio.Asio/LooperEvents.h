#pragma once
#include <ILooperEvents.h>
#include <UnknownBase.h>
#include "Looper.h"

using namespace Audio::Asio::Unmanaged;
using namespace Audio::Asio::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			class LooperEvents : public ILooperEvents
			{
			public:
				LooperEvents(Looper^ looper);
				virtual ~LooperEvents();

				void Heartbeat(ILooper& looper, ITransportPtr& transport);

				void TransportStatusChanged(ILooper& looper, TransportCode previous, TransportCode current);

				void LoopRestart(ILooper& looper);

				void LoopRecordingChanged(ILooper& looper, bool isLoopRecording);

				void SessionRecordingChanged(ILooper& looper, bool isSessionRecording);

				void AddLoop(ILooper& looper);

				void DropRecording(ILooper& looper, bool continueRecording);

				DECLARE_IUNKNOWN

			private:
				gcroot<Looper^> m_looper;
			};
		}
	}
}