#pragma once

#include "BufferSwitchManagedCallback.h"
#include <IInputChannel.h>
#include <IOutputChannelPair.h>

using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		class AsioCore;

		namespace Interop
		{
			public ref class AsioDevice sealed : public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				static initonly System::String^ IsPoweredOnProperty = gcnew System::String("IsPoweredOn");

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

				static AsioDevice^ CreateFromGuid(System::Guid guid);

				AsioDevice(AsioCore* pCore);
				~AsioDevice();
				!AsioDevice();

				void ShowControlPanel();

				property System::String^ DriverName
				{
					System::String^ get();
				}

				property int DriverVersion
				{
					int get();
				}

				property int InputChannelCount
				{
					int get();
				}

				property int OutputChannelPairCount
				{
					int get();
				}

				property System::Boolean IsPoweredOn
				{
					System::Boolean get();
					void set(System::Boolean value);
				}

			internal:
				property int SampleCount
				{
					int get();
				}

				property int SampleRate
				{
					int get();
					void set(int value);
				}

				void AttachBufferSwitchHandler(BufferSwitchManagedCallback^ bufferSwitchHandler);
				void DetachBufferSwitchHandler();

				IInputChannel* GetInputChannel(int index);
				IOutputChannelPair* GetOutputChannelPair(int index);

			private:
				void CleanUp(bool fIsDisposing);
				void OnPropertyChanged(System::String^ propertyName);

				AsioCore* m_pCore;
				bool m_isPoweredOn;
				System::Runtime::InteropServices::GCHandle m_delegateHandle;
				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
