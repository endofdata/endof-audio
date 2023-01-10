#pragma once

#include <IMeterChannel.h>
#include <IInputChannel.h>

using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			delegate void MeterUpdateDelegate(System::IntPtr pSender);

			public ref class AudioInput sealed : public Audio::Foundation::Interop::AudioInputBase
			{
			public:

				property Level DbFS
				{
					virtual Level get() override;
				}

			internal:
				AudioInput(int sampleRate, IInputChannelPtr pChannel, int id);
				virtual ~AudioInput();
				!AudioInput();

			protected:
				virtual bool OnSetMonitor(IAudioOutput^ value) override;
				virtual bool OnAddTarget(IAudioOutput^ target) override;
				virtual void OnRemoveTarget(IAudioOutput^ target) override;

			private:
				void InputMeter_MeterUpdate(System::IntPtr sender);
				void CleanUp(bool isDisposing);

				bool m_isDisposed;
				IMeterChannel* m_pInputMeter;
				IInputChannel* m_pInputChannel;
				System::Runtime::InteropServices::GCHandle m_meterUpdateDelegateHandle;
			};
		}
	}
}
