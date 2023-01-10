#pragma once

#include "Level.h"
#include "IAudioOutput.h"

using namespace Audio::Foundation::Abstractions;
using namespace System::ComponentModel;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioOutputBase abstract : public System::ComponentModel::INotifyPropertyChanged, public IAudioOutput
			{
			public:
				static initonly System::String^ ChannelIdProperty = gcnew System::String("ChannelId");
				static initonly System::String^ DbFSProperty = gcnew System::String("DbFS");

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

				AudioOutputBase(int channelId);
				virtual ~AudioOutputBase();

				property int ChannelId
				{
					virtual int get();
				}

				property Level DbFS
				{
					virtual Level get() abstract;
				}

			protected:
				virtual void OnPropertyChanged(System::String^ propertyName);

			private:
				int m_channelId;

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
