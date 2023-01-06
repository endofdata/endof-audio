#pragma once

#include "FileAudioTake.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioRecording sealed : public FileAudioTake
			{
			public:
				AudioRecording(System::TimeSpan offset, int sampleRate, System::String^ wavStreamFileName);
				virtual ~AudioRecording();

				void WriteNextFrame(cli::array<System::Single>^ audioData);

				bool Finish();

			protected:
				virtual Audio::Foundation::Interop::WaveFile^ OpenWaveFile(System::String^ fileName) override;

			private:
				System::IO::Stream^ m_stream;
				System::Threading::Mutex^ m_lock;
			};
		}
	}
}