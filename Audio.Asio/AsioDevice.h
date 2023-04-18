#pragma once

#include <IInputChannel.h>
#include <IOutputChannelPair.h>
#include <AsioCore.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class AsioDevice sealed : public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				/// <summary>
				/// Gets the name of the <see cref="IsPoweredOn"/> property
				/// </summary>
				static initonly System::String^ IsPoweredOnProperty = gcnew System::String("IsPoweredOn");

				/// <summary>
				/// Gets the name of the <see cref="OutputSaturation"/> property
				/// </summary>
				static initonly System::String^ OutputSaturationProperty = gcnew System::String("OutputSaturation");

				/// <summary>
				/// Implements the <see cref="System::ComponentModel::INotifyPropertyChanged"/> handling.
				/// </summary>
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

				/// <summary>
				/// Creates an instance from a device driver <paramref name="guid"/>
				/// </summary>
				/// <param name="guid">Unique identifier of the device driver.</param>
				/// <returns>New instance of ASIO device</returns>
				/// <seealso cref="CreateFromName"/>
				/// <seealso cref="CreateFromNameLike"/>
				static AsioDevice^ CreateFromGuid(System::Guid guid);

				/// <summary>
				/// Creates an instance from an excact driver <paramref name="name"/>
				/// </summary>
				/// <param name="name">Exact name of the ASIO driver</param>
				/// <returns>New instance of ASIO device</returns>
				/// <seealso cref="CreateFromNameLike"/>
				/// <seealso cref="CreateFromGuid"/>
				static AsioDevice^ CreateFromName(System::String^ name);

				/// <summary>
				/// Creates an instance from driver name <paramref name="pattern"/>
				/// </summary>
				/// <param name="name">Regular expression to match against the ASIO driver name.</param>
				/// <returns>New instance of ASIO device</returns>
				/// <seealso cref="CreateFromNameLike"/>
				/// <seealso cref="CreateFromGuid"/>
				static AsioDevice^ CreateFromNameLike(System::String^ pattern);

				/// <summary>
				/// Constructor
				/// </summary>
				/// <param name="pCore">ASIO core instance</param>
				AsioDevice(Audio::Asio::Unmanaged::AsioCore* pCore);
								
				~AsioDevice();
				!AsioDevice();

				/// <summary>
				/// Shows the device-specific control panel dialog.
				/// </summary>
				void ShowControlPanel();

				/// <summary>
				/// Gets the driver name
				/// </summary>
				property System::String^ DriverName
				{
					System::String^ get();
				}

				/// <summary>
				/// Gets the driver version
				/// </summary>
				property int DriverVersion
				{
					int get();
				}

				property System::Collections::Generic::IDictionary<int, System::String^>^ AvailableInputChannels
				{
					System::Collections::Generic::IDictionary<int, System::String^>^ get();
				}

				property System::Collections::Generic::IDictionary<int, System::String^>^ AvailableOutputChannels
				{
					System::Collections::Generic::IDictionary<int, System::String^>^ get();
				}

				bool SelectInputChannel(int channelId, bool isSelected);

				bool SelectOutputChannel(int channelId, bool isSelected);

				void ActivateChannels();

				/// <summary>
				/// Gets the number of active input channels
				/// </summary>
				property int InputChannelCount
				{
					int get();
				}

				/// <summary>
				/// Gets the number of active ouput channel pairs
				/// </summary>
				property int OutputChannelPairCount
				{
					int get();
				}

				/// <summary>
				/// Gets or sets a value that indicates, whether the device is currently started or not
				/// </summary>
				/// <remarks>
				/// Setting this to <see langword="true"/> invokes <see cref="AsioCore.Start"/> on the underlying
				/// ASIO core instance. Setting this to <see langword="false"/> invokes <see cref="AsioCore.Stop"/>.
				property System::Boolean IsPoweredOn
				{
					System::Boolean get();
					void set(System::Boolean value);
				}

				property System::Single OutputSaturation
				{
					System::Single get();
					void set(System::Single value);
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

				IInputChannelPtr GetInputChannel(int channelId);
				IOutputChannelPairPtr GetOutputChannelPair(int channelPairId);

			private:
				void CleanUp(bool fIsDisposing);
				void OnPropertyChanged(System::String^ propertyName);

				Audio::Asio::Unmanaged::AsioCore* m_pCore;
				bool m_isPoweredOn;
				System::Single m_outputSaturation;
				System::Runtime::InteropServices::GCHandle m_delegateHandle;
				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
				System::Collections::Generic::List<int>^ m_activeInputChannelIds;
				System::Collections::Generic::List<int>^ m_activeOutputChannelIds;
			};
		}
	}
}
