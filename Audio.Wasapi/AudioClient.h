#pragma once
#include "pch.h"

namespace Audio
{
	namespace Wasapi
	{
		ref class CaptureClient;

		// REFERENCE_TIME time units per second and per millisecond
		const int REFTIMES_PER_SEC = 10000000;
		const int REFTIMES_PER_MILLISEC = 10000;

		public ref class AudioClient
		{
		internal:
			AudioClient(IAudioClient* pUnmanaged);
			virtual ~AudioClient();
			!AudioClient();

			property WAVEFORMATEX& MixFormat
			{
				WAVEFORMATEX& get();
			}

			property int BufferSize
			{
				int get();
			}

			void Initialize();
			void Initialize(double requestDurationMs);
			void Initialize(double requestDurationMs, WAVEFORMATEX& format);
			void Initialize(double requestDurationMs, WAVEFORMATEX& format, bool isExclusive);

			CaptureClient^ CreateCaptureClient();

			void Start();
			void Stop();

			void RecordingLoop(System::Action<System::ReadOnlySpan<byte>, int>^ callback);

		private:
			void CleanUp();
			void UpdateMixFormat();

			WAVEFORMATEX* m_pMixFormat;

			IAudioClient* m_pUnmanaged;
		};
	}
}