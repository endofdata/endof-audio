#pragma once

#include "LooperConfig.h"
#include <ILooper.h>

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
				Recording
			};

			public ref class Looper sealed : public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				static initonly System::String^ IsLoopingProperty = gcnew System::String("IsLooping");
				static initonly System::String^ RecordingStatusProperty = gcnew System::String("RecordingStatus");
				static initonly System::String^ IsSessionRecordingProperty = gcnew System::String("IsSessionRecording");
				static initonly System::String^ LoopCountProperty = gcnew System::String("LoopCount");
				static initonly System::String^ LoopLengthProperty = gcnew System::String("LoopLength");
				static initonly System::String^ LoopPositionProperty = gcnew System::String("LoopPosition");
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

				static Looper^ Create(LooperConfig^ config);

				~Looper();

				bool SelectInput(int input, bool isSelected);
				bool SelectOutputPair(int left, int right, bool isSelected);

				void Run();

				void SaveSession(System::String^ filenameBase);

				System::String^ AddVstPlugin(System::String^ vstLibrary);

				int InsertFx(System::String^ pluginId);

				//int InsertFx(ISampleProcessorPtr effect);
				//bool RemoveFx(int id);

				property bool IsLooping
				{
					bool get();
				}

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

			internal:
				Looper(Audio::Asio::Unmanaged::Abstractions::ILooper* inner);

				void OnRecordingStatusChanged(RecordingMode value);
				void OnLoopRestart();

			private:
				void OnPropertyChanged(System::String^ propertyName);

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
				Audio::Asio::Unmanaged::Abstractions::ILooper* _unmanaged;
				float _loopPosition;
				int _loopLength;
			};
		}
	}
}