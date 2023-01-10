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
			public ref class AudioInputBase abstract //: public ::Audio::Foundation::Abstractions::IAudioInput
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
					virtual Level get() = 0;
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

				property System::Collections::Generic::IEnumerable<IAudioOutput^>^ Targets
				{
					virtual System::Collections::Generic::IEnumerable<IAudioOutput^>^ get();
				}

				virtual bool AddTarget(IAudioOutput^ target);
				virtual bool RemoveTarget(IAudioOutput^ target);
				virtual void RemoveAllTargets();

			protected:
				virtual bool OnSetMonitor(IAudioOutput^ monitor) = 0;
				virtual bool OnAddTarget(IAudioOutput^ target) = 0;
				virtual void OnRemoveTarget(IAudioOutput^ target) = 0;

			private:
				int m_channelId;
				bool m_isActive;
				IAudioOutput^ m_monitor;
				System::Collections::Generic::List<IAudioOutput^>^ m_targets;
			};
		}
	}
}
