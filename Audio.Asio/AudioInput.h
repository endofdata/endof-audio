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

			public ref class AudioInput sealed : public Audio::Foundation::Interop::AudioInputBase, public ::Audio::Foundation::Abstractions::IAudioInput, public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				static initonly System::String^ DbFSProperty = gcnew System::String("DbFS");
				static initonly System::String^ IsActiveProperty = gcnew System::String("IsActive");
				static initonly System::String^ MonitorProperty = gcnew System::String("Monitor");


				property Level DbFS
				{
					virtual Level get() override;
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

			internal:
				AudioInput(int sampleRate, IInputChannel* pChannel, int id);
				virtual ~AudioInput() override;
				!AudioInput();

			protected:
				virtual bool OnSetMonitor(IAudioOutput^ value) override;
				virtual bool OnAddTarget(IAudioOutput^ target) override;
				virtual void OnRemoveTarget(IAudioOutput^ target) override;

			private:
				void InputMeter_MeterUpdate(System::IntPtr sender);
				void OnPropertyChanged(System::String^ propertyName);
				void CleanUp(bool isDisposing);

				bool m_isDisposed;
				IMeterChannel* m_pInputMeter;
				IInputChannel* m_pInputChannel;
				System::Runtime::InteropServices::GCHandle m_meterUpdateDelegateHandle;

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
