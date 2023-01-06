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

				/// <summary>
				/// Adds a new audio track.
				/// </summary>
				/// <returns>New audio track.</returns>
				/// <remarks>
				/// The track is added to the <see cref="Tracks"/> collection and its <see cref="AudioTrack.MonitorOut"/>
				/// is routed to the first output pair of <see cref="Router"/>
				/// </remarks>
				AudioTrack^ AddTrack();

				/// <summary>
				/// Removes the audio track with the specified <paramref name="trackId"/> from the <see cref="Tracks"/> collection.
				/// </summary>
				/// <param name="trackId">Id of the track to remove.</param>
				void RemoveTrack(int trackId);
				/// <summary>
				/// Removes the specified audio <paramref name="track"/> from the <see cref="Tracks"/> collection.
				/// </summary>
				void RemoveTrack(AudioTrack^ track);

				/// <summary>
				/// Waits for the tape machine to go idle.
				/// </summary>
				/// <returns><see langword="true"/> if idle event was received, otherwise <see langword="false"/>.</returns>
				/// <remarks>
				/// Setting <see cref="IsRunning"/> to <see langword="false"/> raises an idle event. Also, when the tape machine 
				/// is not recording and no more audio data to output is available, an idle event is raised.
				/// </remarks>
				bool WaitForIdle();
				/// <summary>
				/// Waits up to the specified <paramref name="timeout"/> for the tape machine to go idle.
				/// </summary>
				/// <param name="timeout">Return if no idle event was received during this time span.</param>
				/// <returns><see langword="true"/> if idle event was received, otherwise <see langword="false"/>.</returns>
				bool WaitForIdle(System::TimeSpan timeout);
				/// <summary>
				/// Waits up to the specified <paramref name="timeout"/> for the tape machine to go idle.
				/// </summary>
				/// <param name="timeout">Return if no idle event was received during this time span.</param>
				/// <param name="exitContext">Controls synchronisation context handling.</param>
				/// <returns><see langword="true"/> if idle event was received, otherwise <see langword="false"/>.</returns>
				/// <seealso cref="System.Threading.ManualResetEvent.WaitOne"/>
				bool WaitForIdle(System::TimeSpan timeout, bool exitContext);

				/// <summary>
				/// Gets or sets the directory in which temporary audio files are stored.
				/// </summary>
				/// <remarks>
				/// The default value is obtained by calling <see cref="System.IO.Path.GetTempPath"/>. 
				/// </remarks>
				/// <exception cref="System.ArgumentException">Thrown if an attempt was made to set this property to a non-existing path.</exception>
				property System::String^ TempAudioPath
				{
					System::String^ get();
					void set(System::String^ value);
				}

				/// <summary>
				/// Gets the collection of audio tracks.
				/// </summary>
				/// <seealso cref="AddTrack"/>
				/// <seealso cref="RemoveTrack(int)"/>
				/// <seealso cref="RemoveTrack(AudioTrack)"/>
				property AudioTrackCollection^ Tracks
				{
					AudioTrackCollection^ get();
				}

				/// <summary>
				/// Gets or sets the current playback position.
				/// </summary>
				/// <exception cref="InvalidOperationException">Thrown if an attempt was made to set the position while the tape machine is running.</exception>
				property System::TimeSpan Position
				{
					System::TimeSpan get();
					void set(System::TimeSpan value);
				}

				/// <summary>
				/// Gets or sets a value indicating whether the tape machine's virtual transport is running.
				/// </summary>
				/// <remarks>
				/// Setting this property to <see langword="true"/> resets the idle event, prepares the tracks and starts the virtual transport.
				/// Setting this property to <see langword="false"/> stops the virtual transport and raises the idle event.
				/// </remarks>
				property bool IsRunning
				{
					bool get();
					void set(bool value);
				}

				/// <summary>
				/// Gets or sets a value indicating whether the tape machine is in recording mode.
				/// </summary>
				/// <remarks>
				/// Setting this property to <see langword="true"/> invokes <see cref="AudioTrack.BeginRecording"/> for all tracks.
				/// Setting this property to <see langword="false"/> invokes <see cref="AudioTrack.EndRecording"/> for all tracks.
				/// </remarks>
				property bool IsRecording
				{
					bool get();
					void set(bool value);
				}

				/// <summary>
				/// Gets the audio router instance which was passed to the constructor of this tape machine instance.
				/// </summary>
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
				bool m_isRunning;
				bool m_isRecording;
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
