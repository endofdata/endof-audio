#pragma once


namespace Audio
{
	namespace Asio
	{
		class AsioCore;
		class InputInt32Channel;

		namespace Interop
		{
			ref class AsioRouter;
			ref class AudioTrack;
			ref class AudioTrackCollection;			

			public ref class TapeMachine sealed : public System::ComponentModel::INotifyPropertyChanged
			{
			public:
				static initonly System::Diagnostics::TraceSource^ TraceSource = gcnew System::Diagnostics::TraceSource(TapeMachine::typeid->Name);

				static initonly System::String^ PositionProperty = gcnew System::String("Position");
				static initonly System::String^ IsRunningProperty = gcnew System::String("IsRunning");
				static initonly System::String^ IsRecordingProperty = gcnew System::String("IsRecording");
				static initonly System::String^ TempAudioPathProperty = gcnew System::String("TempAudioPath");

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

				virtual event System::EventHandler<System::EventArgs^>^ Idle;

				TapeMachine(AsioRouter^ router);
				~TapeMachine();
				!TapeMachine();

				AudioTrack^ AddTrack();

				void RemoveTrack(int trackId);
				void RemoveTrack(AudioTrack^ track);

				bool WaitForIdle();
				bool WaitForIdle(System::TimeSpan timeout);
				bool WaitForIdle(System::TimeSpan timeout, bool exitContext);

				property System::String^ TempAudioPath
				{
					System::String^ get();
					void set(System::String^ value);
				}

				property AudioTrackCollection^ Tracks
				{
					AudioTrackCollection^ get();
				}

				property System::TimeSpan Position
				{
					System::TimeSpan get();
					void set(System::TimeSpan value);
				}

				property System::Boolean IsRunning
				{
					System::Boolean get();
					void set(System::Boolean value);
				}

				property System::Boolean IsRecording
				{
					System::Boolean get();
					void set(System::Boolean value);
				}

				property AsioRouter^ Router
				{
					AsioRouter^ get();
				}

			internal:
				void OnTrackStatusChanged(AudioTrack^ sender);

				System::String^ GetTempAudioFileName();

				void OnInputBufferSwitch(bool isSecondHalf);
				void OnOutputBufferSwitch(bool isSecondHalf);

			private:
				void CleanUp(bool fIsDisposing);

				void OnPropertyChanged(System::String^ propertyName);
				void OnIdle();
				void PrepareTracks();

				int m_nextTrackId;
				System::String^ m_tempAudioPath;
				AsioRouter^ m_router;
				AudioTrackCollection^ m_audioTracks;
				System::TimeSpan m_position;
				System::Boolean m_isRunning;
				System::Boolean m_isRecording;
				void AudioTracks_CollectionChanged(System::Object^ sender, System::Collections::Specialized::NotifyCollectionChangedEventArgs^ e);
				System::UInt32 m_recordings;
				System::Diagnostics::Stopwatch^ m_stopWatch;
				System::Threading::ManualResetEvent^ m_idleEvent;
				bool m_isIdleSignaled;

				// Lambdas
				static inline bool TrackIsSoloed(AudioTrack^ t);

				System::ComponentModel::PropertyChangedEventHandler^ m_propertyChangedEventHandler;
			};
		}
	}
}
