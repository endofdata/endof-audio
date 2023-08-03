#pragma once

#include "ManagedLooperConfig.h"
#include <ILooper.h>
#include "LooperEvents.h"

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public enum class RecordingMode
			{
				Off,
				Armed,
				Recording,
				Unarmed
			};

			public ref class ManagedLooper sealed : public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				static initonly System::String^ IsLoopingProperty = gcnew System::String("IsLooping");
				static initonly System::String^ RecordingStatusProperty = gcnew System::String("RecordingStatus");
				static initonly System::String^ IsSessionRecordingProperty = gcnew System::String("IsSessionRecording");
				static initonly System::String^ LoopCountProperty = gcnew System::String("LoopCount");
				static initonly System::String^ LoopLengthProperty = gcnew System::String("LoopLength");
				static initonly System::String^ LoopPositionProperty = gcnew System::String("LoopPosition");
				static initonly System::String^ TransportPositionProperty = gcnew System::String("TransportPosition");
				static initonly System::String^ NameProperty = gcnew System::String("Name");

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

				static ManagedLooper^ Create(ManagedLooperConfig^ config);

				~ManagedLooper();

				bool SelectInput(int input, bool isSelected);
				bool SelectOutputPair(int left, int right, bool isSelected);

				void Run();

				void SaveSession(System::String^ filenameBase);

				System::String^ AddVstPlugin(System::String^ vstLibrary);

				int InsertFx(System::String^ pluginId);

				//int InsertFx(ISampleProcessorPtr effect);
				//bool RemoveFx(int id);

				property RecordingMode RecordingStatus
				{
					RecordingMode get();
				}

				property float LoopPosition
				{
					float get();
				}

				property int LoopLength
				{
					int get();
				}

				property int LoopCount
				{
					int get();
				}

				property bool IsLooping
				{
					bool get();
				}

				property bool IsSessionRecording
				{
					bool get();
					void set(bool value);
				}

				property System::String^ Name
				{
					System::String^ get();
					void set(System::String^ value);
				}

				property Audio::Foundation::Interop::ManagedAudioTime^ TransportPosition
				{
					Audio::Foundation::Interop::ManagedAudioTime^ get();
					void set(Audio::Foundation::Interop::ManagedAudioTime^ value);
				}

			internal:
				ManagedLooper(Audio::Asio::Unmanaged::Abstractions::ILooper* inner);

				void OnPropertyChanged(System::String^ propertyName);

			private:

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
				Audio::Asio::Unmanaged::Abstractions::ILooper* _unmanaged;
				LooperEvents* _events;
				int _loopLength;
				float _loopPosition;
				Audio::Foundation::Interop::ManagedAudioTime^ _transportPosition;
			};
		}
	}
}