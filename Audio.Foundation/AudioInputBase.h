#pragma once

#include "IAudioInput.h"
#include "IAudioOutput.h"

using namespace Audio::Foundation::Abstractions;
using namespace System::ComponentModel;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioInputBase abstract : public ::Audio::Foundation::Abstractions::IAudioInput, public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				static initonly System::String^ ChannelIdProperty = gcnew System::String("ChannelId");
				static initonly System::String^ DbFSProperty = gcnew System::String("DbFS");
				static initonly System::String^ IsActiveProperty = gcnew System::String("IsActive");
				static initonly System::String^ MonitorProperty = gcnew System::String("Monitor");
				static initonly System::String^ TargetsProperty = gcnew System::String("Targets");

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
				virtual void OnPropertyChanged(System::String^ propertyName);

			private:
				int m_channelId;
				bool m_isActive;
				IAudioOutput^ m_monitor;
				System::Collections::Generic::List<IAudioOutput^>^ m_targets;

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
