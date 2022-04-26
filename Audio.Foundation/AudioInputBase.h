#pragma once

#include "IAudioInput.h"
#include "IAudioOutput.h"

using namespace Audio::Foundation::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioInputBase abstract : public ::Audio::Foundation::Abstractions::IAudioInput
			{
			public:
				AudioInputBase(int channelId);
				virtual ~AudioInputBase();

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

				property System::Boolean IsActive
				{
					virtual System::Boolean get();
					virtual void set(System::Boolean value) ;
				}

				property IAudioOutput^ Monitor
				{
					virtual IAudioOutput^ get();
					virtual void set(IAudioOutput^ value);
				}

				virtual void ReadCurrentFrame(cli::array<System::Single>^ frameBuffer) = 0;

			private:
				int m_channelId;
				Level m_dbFS;
				bool m_isActive;
				IAudioOutput^ m_monitor;
			};
		}
	}
}
