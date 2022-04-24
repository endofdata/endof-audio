#pragma once
#include "pch.h"

namespace Audio
{
	namespace Wasapi
	{
		public ref class CaptureClient
		{
		internal:
			CaptureClient(IAudioCaptureClient* pUnmanaged, WAVEFORMATEX& mixFormat);
			virtual ~CaptureClient();
			!CaptureClient();

			property int NextPacketLength
			{
				int get();
			}

			bool Read(System::Action<System::ReadOnlySpan<byte>, int>^ callback);

		private:
			void CleanUp();

			IAudioCaptureClient* m_pUnmanaged;
			int m_frameSize;
		};
	}
}