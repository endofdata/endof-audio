#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <ITransportEvents.h>
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
				__interface _AUDIO_ASIO_UNMANAGED_API  ILooper;

				__interface _AUDIO_ASIO_UNMANAGED_API  _declspec(uuid("98bd51b8-8ab7-4a58-825b-70f77c447675")) ILooperEvents : public IUnknown
				{
					void Heartbeat(ILooper& looper, ITransportPtr& transport) = 0;

					void TransportStatusChanged(ILooper& looper, TransportCode previous, TransportCode current) = 0;

					void LoopRestart(ILooper& looper) = 0;

					void LoopRecordingChanged(ILooper& looper, bool isLoopRecording) = 0;

					void SessionRecordingChanged(ILooper& looper, bool isSessionRecording) = 0;

					void AddLoop(ILooper& looper) = 0;

					void DropRecording(ILooper& looper, bool continueRecording) = 0;
				};

				_AUDIO_ASIO_UNMANAGED_API  _COM_SMARTPTR_TYPEDEF(ILooperEvents, __uuidof(ILooperEvents));
			}
		}
	}
}