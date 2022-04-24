#pragma once

#include "AudioTakeBase.h"
#include "SampleConversion.h"

using namespace System;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			ref class WaveFile;

			public ref class FileAudioTake : public AudioTakeBase
			{
			public:
				FileAudioTake(System::TimeSpan offset, int sampleRate, System::String^ wavStreamFileName);
				virtual ~FileAudioTake();

				property System::String^ Filename
				{
					System::String^ get();
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

			protected:
				
				property WaveFile^ WavFile
				{
					WaveFile^ get();
					void set(WaveFile^ value);
				}

				virtual WaveFile^ OpenWaveFile(System::String^ fileName);

			private:
				void CleanUp();
				void Prepare();

				System::String^ m_fileName;
				WaveFile^ m_waveFile;
			};
		}
	}
}