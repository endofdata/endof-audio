#pragma once

#include "AudioTakeBase.h"

namespace Audio
{
	namespace Foundation
	{
		ref class WaveFile;

		namespace Interop
		{
			public ref class StreamAudioTake : public AudioTakeBase
			{
			public:
				StreamAudioTake(System::TimeSpan offset, int sampleRate, int sampleSize, System::IO::Stream^ sampleStream);
				StreamAudioTake(System::TimeSpan offset, int sampleRate, int sampleSize, System::IO::Stream^ sampleStream, bool leaveOpen);
				virtual ~StreamAudioTake();

				property System::IO::Stream^ SampleStream
				{
					System::IO::Stream^ get();
				}

				property System::TimeSpan Position
				{
					virtual System::TimeSpan get() override;
					virtual void set(System::TimeSpan value) override;
				}

				property System::TimeSpan Length
				{
					virtual System::TimeSpan get() override;
				}

				virtual int ReadNextFrame(cli::array<System::Single>^ audioBuffer) override;

			private:
				System::IO::Stream^ m_sampleStream;
				bool m_leaveOpen;
				int m_bytesPerSecond;
			};
		}
	}
}