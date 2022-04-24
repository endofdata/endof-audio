#pragma once

using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Interop;

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			ref class TapeMachine;

			[System::FlagsAttribute]
			public enum class TrackStatus
			{
				None,
				Muted,
				Solo,
				Hot,
				Recording
			};

			public ref class AudioTrack sealed : public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				static initonly System::String^ IsReadyProperty = gcnew System::String("IsReady");
				static initonly System::String^ IsHotProperty = gcnew System::String("IsHot");
				static initonly System::String^ IsRecordingProperty = gcnew System::String("IsRecording");
				static initonly System::String^ IsMutedProperty = gcnew System::String("IsMuted");
				static initonly System::String^ IsSoloProperty = gcnew System::String("IsSolo");
				static initonly System::String^ RecordInProperty = gcnew System::String("RecordIn");
				static initonly System::String^ MonitorOutProperty = gcnew System::String("MonitorOut");

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

				property System::String^ Name;
				property bool IsSilent;
				property float Volume;
				property float Pan;

				property int TrackId
				{
					int get();
				}

				property TapeMachine^ Machine
				{
					TapeMachine^ get();
				}

				property bool IsReady
				{
					bool get();
				}

				property bool IsHot
				{
					bool get();
					void set(bool value);
				}

				property bool IsRecording
				{
					bool get();
				}
			
				property bool IsMuted
				{
					bool get();
					void set(bool value);
				}

				property bool IsSolo
				{
					bool get();
					void set(bool value);
				}

				property Level DisplayLevel
				{
					Level get();
				}

				property TrackStatus Status
				{
					TrackStatus get();
					void set(TrackStatus value);
				}

				property IAudioInput^ RecordIn
				{
					IAudioInput^ get();
					void set(IAudioInput^ value);
				}

				property IAudioOutput^ MonitorOut
				{
					IAudioOutput^ get();
					void set(IAudioOutput^ value);
				}

				void InsertTake(IAudioTake^ take);

				virtual System::String^ ToString() override;


			internal:
				AudioTrack(TapeMachine^ tapeMachine, int trackId);
				~AudioTrack();

				void Prepare();
				void Done();
				void Abort();

				bool NextFrame();
				void Send();

				void BeginRecording();
				void EndRecording();

				property System::Collections::Generic::IEnumerable<IAudioTake^>^ Takes
				{
					System::Collections::Generic::IEnumerable<IAudioTake^>^ get();
				}

			private:
				void CleanUp();
				void OnPropertyChanged(System::String^ propertyName);

				IAudioTake^ GetNextTake();

				AudioRecording^ m_recordingTake;
				IAudioTake^ m_playbackTake;
				int m_playbackIndex;
				System::Collections::Generic::SortedDictionary<System::TimeSpan, IAudioTake^>^ m_takes;

				System::Threading::CancellationToken m_cancellationToken;
				bool m_isHot;
				bool m_isRecording;
				bool m_isMuted;
				bool m_isSolo;
				bool m_isAtEndOfStream;

				int m_trackId;
				TapeMachine^ m_machine;
				IAudioInput^ m_recordIn;
				IAudioOutput^ m_monitorOut;
				cli::array<System::Single>^ m_pCurrentFrame;

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
