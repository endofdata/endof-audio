#pragma once

#include "Level.h"
#include "IAudioOutput.h"

using namespace Audio::Foundation::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioOutputBase abstract : public ::Audio::Foundation::Abstractions::IAudioOutput
			{
			public:
				AudioOutputBase(int channelId);
				virtual ~AudioOutputBase();

				property int ChannelId
				{
					virtual int get();
					virtual void set(int value);
				}

				property Level DbFS
				{
					virtual Level get();
					virtual void set(Level value);
				}

				virtual void WriteCurrentFrame(cli::array<System::Single>^ frameBuffer, float volume, float pan) = 0;

			private:
				int m_channelId;
				Level m_dbFS;
			};
		}
	}
}
