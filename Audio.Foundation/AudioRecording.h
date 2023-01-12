#pragma once

#include "FileAudioTake.h"
#include "IAudioTarget.h"

using namespace Audio::Foundation::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioRecording : public FileAudioTake, public IAudioTarget
			{
			public:
				AudioRecording(System::TimeSpan offset, int sampleRate, System::String^ wavStreamFileName);
				virtual ~AudioRecording();

				virtual int Write(IAudioBuffer^ buffer);

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