#pragma once

#include "ManagedLooperConfig.h"
#include "ManagedLooperTrack.h"
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

			public ref class LoopEventArgs : public System::EventArgs
			{
			public:
				property System::Guid Id;
				property int ChannelCount;
				property int SamplePosition;
				property int SampleCount;

				LoopEventArgs(System::Guid id, int channelCount, int samplePosition, int sampleCount)
				{
					Id = id;
					ChannelCount = channelCount;
					SamplePosition = samplePosition;
					SampleCount = sampleCount;
				}
			};

			public delegate void LoopEventHandler(System::Object^ sender, LoopEventArgs^ e);

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

				virtual event LoopEventHandler^ LoopAdded
				{
					void add(LoopEventHandler^ value) sealed
					{
						m_addLoopEventHandler = static_cast<LoopEventHandler^>(System::Delegate::Combine(m_addLoopEventHandler, value));
					}

					void remove(LoopEventHandler^ value) sealed
					{
						m_addLoopEventHandler = static_cast<LoopEventHandler^>(System::Delegate::Remove(m_addLoopEventHandler, value));
					}

					void raise(System::Object^ sender, LoopEventArgs^ e)
					{
						LoopEventHandler^ handler = m_addLoopEventHandler;

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

				System::Threading::Tasks::Task^ RunAsync(System::Threading::CancellationToken cancellationToken);

				void Stop();
				void Run();

				void SaveSession(System::String^ filenameBase);

				System::String^ AddVstPlugin(System::String^ vstLibrary);

				int InsertFx(System::String^ pluginId);

				//int InsertFx(ISampleProcessorPtr effect);
				//bool RemoveFx(int id);

				bool RemoveLoop(System::Guid id);

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

				ManagedLooperTrack^ GetTrack(System::Guid id);

			internal:
				ManagedLooper(Audio::Asio::Unmanaged::Abstractions::ILooper* inner);

				void OnPropertyChanged(System::String^ propertyName);
				void OnAddLoop(System::Guid id, int channelCount, int samplePosition, int sampleCount);

			private:

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
				LoopEventHandler^ m_addLoopEventHandler;
				Audio::Asio::Unmanaged::Abstractions::ILooper* _unmanaged;
				LooperEvents* _events;
				int _loopLength;
				float _loopPosition;
				Audio::Foundation::Interop::ManagedAudioTime^ _transportPosition;
			};
		}
	}
}