#pragma once

#include <MeterChannel.h>
#include <SampleJoiner.h>
#include <IOutputChannelPair.h>

using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class AudioOutput sealed : public System::ComponentModel::INotifyPropertyChanged, public IAudioOutput
			{
			public:
				property Level DbFS
				{
					virtual Level get();
				}

				property int ChannelId
				{
					virtual int get();
				}

				virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged
				{
					void add(System::ComponentModel::PropertyChangedEventHandler^ value) sealed = INotifyPropertyChanged::PropertyChanged::add
					{
						m_propertyChangedEventHandler = static_cast<System::ComponentModel::PropertyChangedEventHandler^>(System::Delegate::Combine(m_propertyChangedEventHandler, value));
					}

					void remove(System::ComponentModel::PropertyChangedEventHandler^ value) sealed = INotifyPropertyChanged::PropertyChanged::remove
					{
						m_propertyChangedEventHandler = static_cast<System::ComponentModel::PropertyChangedEventHandler^>(System::Delegate::Remove(m_propertyChangedEventHandler, value));
					}

					void raise(System::Object^ sender, System::ComponentModel::PropertyChangedEventArgs^ e)
					{
						System::ComponentModel::PropertyChangedEventHandler^ handler = m_propertyChangedEventHandler;

						if (handler != nullptr)
						{
							handler->Invoke(sender, e);
						}
					}
				}

				virtual void WriteCurrentFrame(cli::array<System::Single>^ frameBuffer, float volume, float pan);

			internal:
				AudioOutput(int sampleRate, int sampleCount, IOutputChannelPair* pChannel, int id);
				~AudioOutput();
				!AudioOutput();

				void Send();

				property IOutputChannelPair& OutputChannelPair
				{
					IOutputChannelPair& get();
				}
			private:
				void CleanUp(bool isDisposing);
				void OnPropertyChanged(System::String^ propertyName);

				int m_channelId;
				SampleJoiner* m_pMasterMix;
				MeterChannel* m_pOutputMeter;
				IOutputChannelPair* m_pOutputChannelPair;

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
