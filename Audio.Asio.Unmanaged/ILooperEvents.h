#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <IControllerEvents.h>
#include <ITransport.h>
#include <comdef.h>


namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				enum class _AUDIO_ASIO_UNMANAGED_API RecordingStatusType
				{
					Off,
					Armed,
					Recording,
					Unarmed
				};

				__interface _AUDIO_ASIO_UNMANAGED_API  ILooper;

				__interface _AUDIO_ASIO_UNMANAGED_API  _declspec(uuid("98bd51b8-8ab7-4a58-825b-70f77c447675")) ILooperEvents : public IUnknown
				{
					void Heartbeat(ILooper& looper, ITransportPtr& transport) = 0;

					void RecordingStatusChanged(ILooper& looper, RecordingStatusType recordingStatus) = 0;

					void LoopRestart(ILooper& looper) = 0;

					void IsLoopingChanged(ILooper& looper, bool isLooping) = 0;

					void IsSessionRecordingChanged(ILooper& looper, bool isSessionRecording) = 0;

					void AddLoop(ILooper& looper, const GUID& id, int channelCount, int samplePosition, int sampleCount) = 0;

					void RemoveLoop(ILooper& looper, const GUID& id) = 0;

					void DropRecording(ILooper& looper) = 0;
				};

				_AUDIO_ASIO_UNMANAGED_API  _COM_SMARTPTR_TYPEDEF(ILooperEvents, __uuidof(ILooperEvents));
			}
		}
	}
}