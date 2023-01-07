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

				/// <summary>
				/// Gets or sets the track name.
				/// </summary>
				/// <remarks>
				/// The default value has the format 'Track n' where 'n' is the track id.
				/// </remarks>
				property System::String^ Name;
				/// <summary>
				/// Gets or sets a value indicating whether the track audio data is sent to its monitor output
				/// </summary>
				property bool IsSilent;
				/// <summary>
				/// Gets or sets the track output level.
				/// </summary>
				/// <remarks>
				/// The default value is 0.7
				/// </remarks>
				property float Level;
				/// <summary>
				/// Gets or sets the track pan position.
				/// </summary>
				property float Pan;

				/// <summary>
				/// Gets the internal track identifier.
				/// </summary>
				property int TrackId
				{
					int get();
				}

				/// <summary>
				/// Gets the tape machine to which the track belongs.
				/// </summary>
				property TapeMachine^ Machine
				{
					TapeMachine^ get();
				}

				/// <summary>
				/// Gets a value indicating whether the track is prepared to record, e.g. <see cref="RecordIn"/> is not <see langword="null"/>.
				/// </summary>
				property bool IsReady
				{
					bool get();
				}

				/// <summary>
				/// Gets or sets a value indicating whether the track is selected for recording.
				/// </summary>
				/// <remarks>
				/// Setting this property also sets the <see cref="IAudioInput.IsActive"/> flag of the assigned
				/// <see cref="RecordIn"/> to the same value. When setting it to <see langword="true"/> 
				/// the next audio track matching the tape machine's playback position is prepared for playback.
				/// Setting it to <see langword="false"/> drops any current recording take before deactivating
				/// <see cref="RecordIn"/>.
				/// </remarks>
				property bool IsHot
				{
					bool get();
					void set(bool value);
				}

				/// <summary>
				/// Gets a value indicating whether the track is currently recording.
				/// </summary>
				/// <remarks>
				/// Calling <see cref="BeginRecording"/> sets this property to <see langword="true"/>. A call to 
				/// <see cref="EndRecording"/> resets it after saving the current recording take, if any.
				property bool IsRecording
				{
					bool get();
				}

				/// <summary>
				/// Gets or sets a value indicating whether the track is currently muted.
				/// </summary>
				property bool IsMuted
				{
					bool get();
					void set(bool value);
				}

				/// <summary>
				/// Gets or sets a value indicating whether the track is currently soloed.
				/// </summary>
				property bool IsSolo
				{
					bool get();
					void set(bool value);
				}

				///// <summary>
				///// Gets the display level which is either the record input or the monitor output depending on <see cref="IsRecording"/>.
				///// </summary>
				//property Level DbFS
				//{
				//	Level get();
				//}

				/// <summary>
				/// Gets or sets the current track status flags.
				/// </summary>
				/// <remarks>
				/// The track status combines the values of <see cref="IsHot"/>, <see cref="IsRecording"/>, <see cref="IsMuted"/> 
				/// and <see cref="IsSolo"/>. Consequently, setting this property with <see cref="TrackStatus.Recording"/> included
				/// calls <see cref="BeginRecording"/> and setting it without <see cref="TrackStatus.Recording"/> calls <see cref="EndRecording"/>.
				/// </remarks>
				property TrackStatus Status
				{
					TrackStatus get();
					void set(TrackStatus value);
				}

				/// <summary>
				/// Gets or sets the audio input from which audio data can be recorded.
				/// </summary>
				property IAudioInput^ RecordIn
				{
					IAudioInput^ get();
					void set(IAudioInput^ value);
				}

				/// <summary>
				/// Gets or sets the audio output to which audio data can be sent.
				/// </summary>
				property IAudioOutput^ MonitorOut
				{
					IAudioOutput^ get();
					void set(IAudioOutput^ value);
				}

				/// <summary>
				/// Inserts an audio take to this track.
				/// </summary>
				/// <param name="take"></param>
				void InsertTake(IAudioTake^ take);

				/// <summary>
				/// Gets a string representation of the audio track.
				/// </summary>
				/// <returns>A display string containing the track name and status codes 'H', 'M' and 'S' or '-' for
				/// <see cref="IsHot"/>, <see cref="IsMuted"/> and <see cref="IsSolo"/>.</returns>
				virtual System::String^ ToString() override;


			internal:
				AudioTrack(TapeMachine^ tapeMachine, int trackId);
				~AudioTrack();

				void Prepare();
				void NewTake();
				void CompleteTake();
				void AbortTake();

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

				bool AdvancePlaybackTake();

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
