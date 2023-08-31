#pragma once
#include <ILooperEvents.h>
#include <UnknownBase.h>

using namespace Audio::Asio::Unmanaged::Abstractions;

namespace RepeatMyself
{
	class LooperEvents : public ILooperEvents
	{
	public:
		LooperEvents();
		virtual ~LooperEvents();

		void Heartbeat(ILooper& looper, ITransportPtr& transport);

		void RecordingStatusChanged(ILooper& looper, RecordingStatusType recordingStatus);

		void LoopRestart(ILooper& looper);

		void IsLoopingChanged(ILooper& looper, bool isLooping);

		void IsSessionRecordingChanged(ILooper& looper, bool isSessionRecording);

		void AddLoop(ILooper& looper, int channelCount, int samplePosition, int sampleCount);

		void DropRecording(ILooper& looper);

		DECLARE_IUNKNOWN

	private:
		static const wchar_t* GetTransportCodeString(ControllerCode value);
		static const wchar_t* GetRecordingStatusString(RecordingStatusType value);

		int m_loopCount;
	};
}
