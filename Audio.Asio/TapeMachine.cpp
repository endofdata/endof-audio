#include "pch.h"

#include "TapeMachine.h"
#include "AsioCoreException.h"
#include "AsioInteropException.h"
#include "AsioRouter.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioTrack.h"
#include "AudioInputCollection.h"
#include "AudioOutputCollection.h"
#include "AudioTrackCollection.h"

#undef GetTempPath

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace Audio::Asio;
using namespace Audio::Asio::Interop;


TapeMachine::TapeMachine(AsioRouter^ router)
{
	if (router == nullptr)
	{
		throw gcnew ArgumentNullException("Parameter 'router' cannot be null.");
	}

	m_router = router;

	m_nextTrackId = 0;

	m_stopWatch = gcnew Stopwatch();
	m_idleEvent = gcnew ManualResetEvent(true);
	m_isIdleSignaled = false;

	m_tempAudioPath = Path::GetTempPath();

	TraceSource->TraceEvent(TraceEventType::Information, 1, "Temporary audio path set to {0}", m_tempAudioPath);

	m_audioTracks = gcnew AudioTrackCollection();
	m_audioTracks->CollectionChanged += gcnew System::Collections::Specialized::NotifyCollectionChangedEventHandler(this, &TapeMachine::AudioTracks_CollectionChanged);

	// Do not attach buffer switch handler, before initialization is complete: the device may already be running
	m_router->AttachBufferSwitchHandler(gcnew BufferSwitchManagedCallback(this, &TapeMachine::OnInputBufferSwitch), true);
	m_router->AttachBufferSwitchHandler(gcnew BufferSwitchManagedCallback(this, &TapeMachine::OnOutputBufferSwitch), false);
}

TapeMachine::~TapeMachine()
{
	CleanUp(true);
}

TapeMachine::!TapeMachine()
{
	CleanUp(false);
}

void TapeMachine::CleanUp(bool fIsDisposing)
{
	if (m_router != nullptr)
	{
		m_router->DetachBufferSwitchHandler(false);
		m_router->DetachBufferSwitchHandler(true);
	}
	if (m_idleEvent != nullptr)
	{
		m_idleEvent->Set();
		delete m_idleEvent;
	}
}

bool TapeMachine::WaitForIdle()
{
	return m_idleEvent->WaitOne();
}

bool TapeMachine::WaitForIdle(TimeSpan timeout)
{
	return m_idleEvent->WaitOne(timeout);
}

bool TapeMachine::WaitForIdle(TimeSpan timeout, bool exitContext)
{
	return m_idleEvent->WaitOne(timeout, exitContext);
}

AudioTrack^ TapeMachine::AddTrack()
{
	AudioTrack^ result = gcnew AudioTrack(this, m_nextTrackId++);

	result->TrackOut = (IAudioTarget^)m_router->Outputs[0];

	m_audioTracks->Add(result);

	TraceSource->TraceEvent(TraceEventType::Information, 3, "Added new track ID {0}", result->TrackId);

	return result;
}

void TapeMachine::RemoveTrack(int trackId)
{
	AudioTrack^ track = m_audioTracks->Find(trackId);

	if (nullptr != track)
		RemoveTrack(track);
}

void TapeMachine::RemoveTrack(AudioTrack^ track)
{
	if (nullptr == track)
		throw gcnew ArgumentNullException();

	m_audioTracks->Remove(track);

	TraceSource->TraceEvent(TraceEventType::Information, 4, "Removed track ID {0}", track->TrackId);
}

void TapeMachine::PrepareTracks()
{
	for each (AudioTrack ^ track in m_audioTracks)
	{
		track->Prepare();
	}
}

bool TapeMachine::TrackIsSoloed(AudioTrack^ track)
{
	if (nullptr == track)
		throw gcnew ArgumentNullException();

	return track->IsSolo;
}

void TapeMachine::AudioTracks_CollectionChanged(System::Object^ sender, System::Collections::Specialized::NotifyCollectionChangedEventArgs^ e)
{
	for each (AudioTrack^ track in e->NewItems)
	{
		if (m_isRecording)
			track->BeginRecording();
		else
			track->EndRecording();
		//		track->Position = m_position;
	}
}

void TapeMachine::OnTrackStatusChanged(AudioTrack^ sender)
{
	if (nullptr == sender)
		throw gcnew ArgumentNullException();

	TrackStatus trackStatus = sender->Status;

	if (trackStatus.HasFlag(TrackStatus::Solo))
		sender->IsSilent = false;
	else if (trackStatus.HasFlag(TrackStatus::Muted))
		sender->IsSilent = true;
}

void TapeMachine::OnIdle()
{
	if (!m_isIdleSignaled)
	{
		m_isIdleSignaled = true;
		Idle(this, EventArgs::Empty);
	}
	m_idleEvent->Set();
}

void TapeMachine::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

void TapeMachine::OnOutputBufferSwitch(bool isSecondHalf)
{
	if (m_isRunning)
	{
		bool hasOutput = false;

		for each (AudioTrack^ track in m_audioTracks)
		{
			if (track->IsHot == false && track->NextFrame())
			{
				hasOutput = true;
				track->Send();
			}
		}

		if (!hasOutput && !IsRecording)
		{
			OnIdle();
		}
	}
}

void TapeMachine::OnInputBufferSwitch(bool isSecondHalf)
{
	// We process data for each hot track so that input monitoring works
	// even when tape machine is not running or recording
	for each (AudioTrack^ track in m_audioTracks)
	{
		if (track->IsHot == true)
		{
			track->NextFrame();
			track->Send();
		}
	}
}

String^ TapeMachine::GetTempAudioFileName()
{
	long long fileTimeUtc = DateTime::Now.ToFileTimeUtc();
	String^ fileName = String::Format("Take {0}-{1:X}.wav", m_recordings++, fileTimeUtc);

	TraceSource->TraceEvent(TraceEventType::Information, 8, "Temporary audio file: {0}", fileName);

	return Path::Combine(m_tempAudioPath, fileName);
}

String^ TapeMachine::TempAudioPath::get()
{
	return m_tempAudioPath;
}

void TapeMachine::TempAudioPath::set(String^ value)
{
	String^ path = Path::GetFullPath(value);

	if (!Directory::Exists(path))
		throw gcnew ArgumentException(String::Format("Invalid path for temporary audio data: '{0}'", path));

	m_tempAudioPath = value;
}

AsioRouter^ TapeMachine::Router::get()
{
	return m_router;
}

AudioTrackCollection^ TapeMachine::Tracks::get()
{
	return m_audioTracks;
}

TimeSpan TapeMachine::Position::get()
{
	if (m_isRunning)
	{
		return TimeSpan::FromMilliseconds(m_position.TotalMilliseconds + m_stopWatch->ElapsedMilliseconds);
	}
	return m_position;
}

void TapeMachine::Position::set(TimeSpan value)
{
	if (m_isRunning)
		throw gcnew InvalidOperationException("Cannot set position while TapeMachine is running.");

	if (m_position != value)
	{
		m_position = value;

		OnPropertyChanged(PositionProperty);

		TraceSource->TraceEvent(TraceEventType::Information, 5, "Position = {0}", value);
	}
	PrepareTracks();
}

bool TapeMachine::IsRecording::get()
{
	return m_isRecording;
}

void TapeMachine::IsRecording::set(bool value)
{
	if (m_isRecording != value)
	{
		// Here, we propagate the recording mode to the tracks only, when transport is currently running.
		// Otherwise, changing the transport state will start/stop track recordings.
		if (m_isRunning)
		{
			if (value)
			{
				for each (AudioTrack ^ track in m_audioTracks)
					track->BeginRecording();
			}
			else
			{
				for each (AudioTrack ^ track in m_audioTracks)
					track->EndRecording();
			}
		}
		m_isRecording = value;
		TraceSource->TraceEvent(TraceEventType::Information, 6, "IsRecording = {0}", value);

		OnPropertyChanged(IsRecordingProperty);
	}
}

bool TapeMachine::IsRunning::get()
{
	return m_isRunning;
}

void TapeMachine::IsRunning::set(bool value)
{
	if (value != m_isRunning)
	{
		if (value)
		{
			m_isIdleSignaled = false;

			if (!m_idleEvent->Reset())
			{
				throw gcnew InvalidOperationException("Failed to reset Idle event.");
			}

			// If record mode was activated before transport is started, propagate recording mode now
			if (m_isRecording)
			{
				for each (AudioTrack ^ track in m_audioTracks)
					track->BeginRecording();
			}

			m_stopWatch->Start();
		}
		else
		{
			m_stopWatch->Stop();
			m_position = TimeSpan::operator+(m_position, m_stopWatch->Elapsed);
			m_stopWatch->Reset();

			// If record mode has not already been deactivated while transport was running, stop track recordings now
			IsRecording = false;
			OnIdle();
		}

		m_isRunning = value;
		TraceSource->TraceEvent(TraceEventType::Information, 7, "IsRunning = {0}, Position = {1}", value, Position);

		OnPropertyChanged(IsRunningProperty);
	}
}
