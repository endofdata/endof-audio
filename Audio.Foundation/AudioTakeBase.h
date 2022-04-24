#pragma once

#include "IAudioTake.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			ref class WaveFile;

			public ref class AudioTakeBase abstract : public ::Audio::Foundation::Abstractions::IAudioTake
			{
			public:
				AudioTakeBase(System::TimeSpan offset, int sampleRate);
				virtual ~AudioTakeBase();

				property float Volume
				{
					virtual float get();
					virtual void set(float value);
				}

				property System::TimeSpan Offset
				{
					virtual System::TimeSpan get();
					virtual void set(System::TimeSpan value);
				}

				property int SampleRate
				{
					int get();
				}

				property System::TimeSpan End
				{
					virtual System::TimeSpan get();
				}

				virtual bool TrySetPosition(System::TimeSpan position);

				// Abstract
				property System::TimeSpan Position
				{
					virtual System::TimeSpan get() = 0;
					virtual void set(System::TimeSpan value) = 0;
				}

				property System::TimeSpan Length
				{
					virtual System::TimeSpan get() = 0;
				}

				virtual void Initialize();

				virtual int ReadNextFrame(cli::array<float>^ audioBuffer) = 0;

			private:
				System::TimeSpan m_offset;
				int m_sampleRate;
				float m_volume;
			};
		}
	}
}