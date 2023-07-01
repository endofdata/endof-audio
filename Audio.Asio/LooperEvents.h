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

				void RecordingStatusChanged(ILooper& looper, RecordingStatusType recordingStatus);

				void LoopRestart(ILooper& looper);

				void IsLoopingChanged(ILooper& looper, bool isLooping);

				void IsSessionRecordingChanged(ILooper& looper, bool isSessionRecording);

				void AddLoop(ILooper& looper);

				void DropRecording(ILooper& looper, bool continueRecording);

				DECLARE_IUNKNOWN

			private:
				gcroot<Looper^> m_looper;
			};
		}
	}
}