#pragma once

#include "IAudioSource.h"
#include "IAudioTarget.h"

using namespace Audio::Foundation::Abstractions;
using namespace System::ComponentModel;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioSourceBase : 
				public IAudioSource,
				public INotifyPropertyChanged
			{
			public:
				static initonly System::String^ IsActiveProperty = gcnew System::String("IsActive");
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

				AudioSourceBase();
				virtual ~AudioSourceBase();

				property System::Boolean IsActive
				{
					virtual System::Boolean get();
					virtual void set(System::Boolean value);
				}

				property System::Collections::Generic::IEnumerable<IAudioTarget^>^ Targets
				{
					virtual System::Collections::Generic::IEnumerable<IAudioTarget^>^ get();
				}

				virtual bool AddTarget(IAudioTarget^ target);
				virtual bool RemoveTarget(IAudioTarget^ target);
				virtual void RemoveAllTargets();

			protected:
				virtual bool OnAddTarget(IAudioTarget^ target);
				virtual void OnRemoveTarget(IAudioTarget^ target);
				virtual void OnPropertyChanged(System::String^ propertyName);

			private:
				bool m_isActive;
				System::Collections::Generic::List<IAudioTarget^>^ m_targets;

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
