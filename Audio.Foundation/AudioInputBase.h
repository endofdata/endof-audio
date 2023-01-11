#pragma once

#include "IAudioInput.h"
#include "IAudioOutput.h"
#include "AudioSourceBase.h"

using namespace Audio::Foundation::Abstractions;
using namespace System::ComponentModel;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioInputBase abstract : 
				public AudioSourceBase,
				public IAudioInput
			{
			public:
				static initonly System::String^ ChannelIdProperty = gcnew System::String("ChannelId");
				static initonly System::String^ DbFSProperty = gcnew System::String("DbFS");
				static initonly System::String^ MonitorProperty = gcnew System::String("Monitor");

				AudioInputBase(int channelId);
				virtual ~AudioInputBase();

				property int ChannelId
				{
					virtual int get();
					virtual void set(int value);
				}

				property Level DbFS
				{
					virtual Level get() = 0;
				}

				property IAudioOutput^ Monitor
				{
					virtual IAudioOutput^ get();
					virtual void set(IAudioOutput^ value);
				}

			protected:
				virtual bool OnSetMonitor(IAudioOutput^ monitor) = 0;

			private:
				int m_channelId;
				IAudioOutput^ m_monitor;
			};
		}
	}
}
