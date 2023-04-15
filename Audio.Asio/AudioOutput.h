#pragma once

#include <IMeterChannel.h>
#include <IOutputChannelPair.h>

using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class AudioOutput sealed : public Audio::Foundation::Interop::AudioOutputBase
			{
			public:

				property Level DbFS
				{
					virtual Level get() override;
				}

			internal:
				AudioOutput(int sampleRate, int sampleCount, IOutputChannelPairPtr pChannel, int id);
				virtual ~AudioOutput();
				!AudioOutput();

				void Send();

				property IOutputChannelPairPtr OutputChannelPair
				{
					IOutputChannelPairPtr get();
				}
			private:
				void OutputMeter_MeterUpdate(System::IntPtr sender);
				void CleanUp(bool isDisposing);

				bool m_isDisposed;
				IMeterChannel* m_pOutputMeter;
				IOutputChannelPair* m_pOutputChannelPair;

				System::Runtime::InteropServices::GCHandle m_meterUpdateDelegateHandle;
			};
		}
	}
}
