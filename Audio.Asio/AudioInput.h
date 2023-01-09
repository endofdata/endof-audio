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

			public ref class AudioInput sealed : public System::ComponentModel::INotifyPropertyChanged, public Audio::Foundation::Abstractions::IAudioInput
			{
			public:
				static initonly System::String^ DbFSProperty = gcnew System::String("DbFS");
				static initonly System::String^ IsActiveProperty = gcnew System::String("IsActive");
				static initonly System::String^ MonitorProperty = gcnew System::String("Monitor");


				property int ChannelId
				{
					virtual int get();
				}

				property Level DbFS
				{
					virtual Level get();
				}

				property bool IsActive
				{
					virtual bool get();
					virtual void set(bool value);
				}

				property IAudioOutput^ Monitor
				{
					virtual IAudioOutput^ get();
					virtual void set(IAudioOutput^ value);
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

				virtual bool AddTarget(IAudioOutput^ target);

				virtual bool RemoveTarget(IAudioOutput^ target);

				virtual void RemoveAllTargets();

			internal:
				AudioInput(int sampleRate, IInputChannel* pChannel, int id);
				~AudioInput();
				!AudioInput();

			private:
				void InputMeter_MeterUpdate(System::IntPtr sender);
				void OnPropertyChanged(System::String^ propertyName);
				void CleanUp(bool isDisposing);
				void UnlinkTarget(IAudioOutput^ target);

				bool m_isDisposed;
				int m_channelId;
				IMeterChannel* m_pInputMeter;
				IInputChannel* m_pInputChannel;
				System::Collections::Generic::List<IAudioOutput^>^ m_targets;
				System::Runtime::InteropServices::GCHandle m_meterUpdateDelegateHandle;

				IAudioOutput^ m_monitor;

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
