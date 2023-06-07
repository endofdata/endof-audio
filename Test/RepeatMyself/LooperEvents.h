#pragma once
#include <ILooperEvents.h>
#include <UnknownBase.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class LooperEvents : public ILooperEvents
			{
			public:
				LooperEvents();
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
				static const wchar_t* GetTransportCodeString(TransportCode value);

				int m_loopCount;
			};
		}
	}
}