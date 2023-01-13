#pragma once

#include "AudioOutputCollection.h"
#include "AudioInputCollection.h"
#include "AsioDevice.h"

namespace Audio
{
	namespace Asio
	{
		class AsioCore;
		class InputInt32Channel;

		namespace Interop
		{
			public ref class AsioRouter sealed : public System::ComponentModel::INotifyPropertyChanged
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

				AsioRouter(AsioDevice^ outputDevice, AsioDevice^ inputDevice);
				~AsioRouter();
				!AsioRouter();

				property AudioInputCollection^ Inputs
				{
					AudioInputCollection^ get();
				}

				property AudioOutputCollection^ Outputs
				{
					AudioOutputCollection^ get();
				}

				property System::Boolean IsPoweredOn
				{
					System::Boolean get();
					void set(System::Boolean value);
				}

				property AsioDevice^ InputDevice
				{
					AsioDevice^ get();
				}

				property AsioDevice^ OutputDevice
				{
					AsioDevice^ get();
				}

			internal:
				property int SampleCount
				{
					int get();
				}

				property int SampleRate
				{
					int get();
				}
								
			private:
				void CleanUp(bool fIsDisposing);
				void OnPropertyChanged(System::String^ propertyName);

				AsioDevice^ m_inputDevice;
				AsioDevice^ m_outputDevice;

				System::Boolean m_isPoweredOn;
				AudioInputCollection^ m_audioInputs;
				AudioOutputCollection^ m_audioOutputPairs;


				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
#pragma once
