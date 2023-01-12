#include "pch.h"
#include "AudioTrack.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AsioCore.h"
#include "AsioRouter.h"
#include "TapeMachine.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Linq;
using namespace System::Collections::Generic;
using namespace Audio::Asio::Interop;
using namespace Audio::Foundation::Interop;
using namespace Audio::Foundation::Abstractions;

AudioTrack::AudioTrack(TapeMachine^ machine, int trackId) :
	m_trackId(trackId),
	m_isAtEndOfStream(false),
	m_machine(machine)
{
	Name = String::Format("Track {0}", trackId);
	IsSilent = false;
	Level = 0.7f;
	Pan = 0.0f;
	IsHot = false;
	IsMuted = false;
	IsSolo = false;
	Status = TrackStatus::None;

	m_takes = gcnew SortedDictionary<TimeSpan, IAudioTake^>();
}

AudioTrack::~AudioTrack()
{
	CleanUp();
}

void AudioTrack::CleanUp()
{
	IsHot = false;

	for each (IAudioTake ^ take in m_takes->Values)
		delete take;

	m_takes->Clear();
}

int AudioTrack::TrackId::get()
{
	return m_trackId;
}

TapeMachine^ AudioTrack::Machine::get()
{
	return m_machine;
}

IAudioSource^ AudioTrack::Source::get()
{
	return m_source;
}

void AudioTrack::Source::set(IAudioSource^ value)
{
	if (value != m_source)
	{
		bool wasHot = IsHot;

		IsHot = false;

		m_source = value;

		OnPropertyChanged(SourceProperty);
		OnPropertyChanged(IsReadyProperty);

		if (nullptr == value)
		{
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 101, "Trk {0}: RecordIn = null", m_trackId);
		}
		else
		{
			IsHot = wasHot;

			IAudioInput^ hwInput = (IAudioInput^)value;
			String^ channelId = hwInput == nullptr ? "-" : hwInput->ChannelId.ToString();
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 101, "Trk {0}: RecordIn = channel {1}", m_trackId, channelId);
		}
	}
}

IAudioTarget^ AudioTrack::Target::get()
{
	return m_target;
}

void AudioTrack::Target::set(IAudioTarget^ value)
{
	if (value != m_target)
	{
		bool wasHot = IsHot;

		IsHot = false;

		m_target = value;

		OnPropertyChanged(TargetProperty);

		if (nullptr == value)
		{
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 102, "Trk {0}: MonitorOut = null", m_trackId);
		}
		else
		{
			IsHot = wasHot;

			IAudioOutput^ hwOutput = (IAudioOutput^)value;
			String^ channelId = hwOutput == nullptr ? "-" : hwOutput->ChannelId.ToString();
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 102, "Trk {0}: MonitorOut = channel pair {1}", m_trackId, channelId);
		}
	}
}



//Level AudioTrack::DbFS::get()
//{
//	Level result;
//
//	if (IsHot)
//		result = m_recordIn->DbFS;
//	else if (nullptr != m_monitorOut)
//		result = m_monitorOut->DbFS;
//	else
//		result = Level(System::Double::NegativeInfinity, System::Double::NegativeInfinity);
//
//	return result;
//}

TrackStatus AudioTrack::Status::get()
{
	TrackStatus status = TrackStatus::None;

	if (m_isHot)
		status = status | TrackStatus::Hot;
	if (m_isRecording)
		status = status | TrackStatus::Recording;
	if (m_isMuted)
		status = status | TrackStatus::Muted;
	if (m_isSolo)
		status = status | TrackStatus::Solo;

	return status;
}

void AudioTrack::Status::set(TrackStatus value)
{
	IsHot = value.HasFlag(TrackStatus::Hot);

	if (value.HasFlag(TrackStatus::Recording))
		BeginRecording();
	else
		EndRecording();

	IsMuted = value.HasFlag(TrackStatus::Muted);
	IsSolo = value.HasFlag(TrackStatus::Solo);
}

bool AudioTrack::IsReady::get()
{
	return m_source != nullptr;
}

bool AudioTrack::IsHot::get()
{
	return m_isHot;
}

void AudioTrack::IsHot::set(bool value)
{
	if (m_isHot != value)
	{
		if (true == value)
		{
			if (!IsReady)
			{
				throw gcnew InvalidOperationException("Before setting the track hot, the RecordIn property must be initialized.");
			}

			m_isHot = value;
			m_source->IsActive = true;
			
			LinkThrough();
			Prepare();
		}
		else
		{
			AbortTake();

			if (IsReady)
			{
				m_source->IsActive = false;
				UnlinkThrough();
			}
			m_isHot = value;

			EndRecording();
		}
		OnPropertyChanged(IsHotProperty);

		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 103, "Trk {0}: IsHot = {1}", m_trackId, value);
	}
}

bool AudioTrack::IsRecording::get()
{
	return m_isRecording;
}

void AudioTrack::BeginRecording()
{
	if (!IsRecording && IsHot)
	{
		m_isRecording = true;
		m_source->AddTarget(m_recordingTake);

		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 104, "Trk {0}: IsRecording = {1}", m_trackId, m_isRecording);

		OnPropertyChanged(IsRecordingProperty);
	}
}

void AudioTrack::EndRecording()
{
	if (IsRecording)
	{
		CompleteTake();
		m_isRecording = false;

		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 104, "Trk {0}: IsRecording = {1}", m_trackId, m_isRecording);

		NewTake();

		OnPropertyChanged(IsRecordingProperty);
	}
}

bool AudioTrack::IsMuted::get()
{
	return m_isMuted;
}

void AudioTrack::IsMuted::set(bool value)
{
	if (m_isMuted != value)
	{
		m_isMuted = value;
		OnPropertyChanged(IsMutedProperty);

		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 105, "Trk {0}: IsMuted = {1}", m_trackId, value);
	}
}

bool AudioTrack::IsSolo::get()
{
	return m_isSolo;
}

void AudioTrack::IsSolo::set(bool value)
{
	if (m_isSolo != value)
	{
		m_isSolo = value;
		OnPropertyChanged(IsSoloProperty);

		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 106, "Trk {0}: IsSolo = {1}", m_trackId, value);
	}
}

System::Collections::Generic::IEnumerable<IAudioTake^>^ AudioTrack::Takes::get()
{
	return m_takes->Values;
}

void AudioTrack::OnPropertyChanged(System::String^ propertyName)
{
	m_machine->OnTrackStatusChanged(this);

	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

bool AudioTrack::LinkThrough()
{
	bool isLinked = false;

	if (m_target != nullptr && m_source != nullptr)
	{
		isLinked = m_source->AddTarget(m_target);
	}
	return isLinked;
}

bool AudioTrack::UnlinkThrough()
{
	bool wasLinked = false;

	if (m_target != nullptr && m_source != nullptr)
	{
		wasLinked = m_source->RemoveTarget(m_target);
	}
	return wasLinked;
}

bool AudioTrack::AdvancePlaybackTake()
{
	if (m_playbackTake != nullptr)
	{
		((IAudioSource^)m_playbackTake)->RemoveTarget(m_target);
	}

	m_playbackTake = m_playbackIndex < m_takes->Count ? m_playbackTake = Enumerable::ElementAtOrDefault(m_takes->Values, m_playbackIndex++) : nullptr;
	m_isAtEndOfStream = nullptr == m_playbackTake && m_playbackIndex >= m_takes->Count;

	if (m_isAtEndOfStream)
	{
		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 107, "Trk {0}: End of stream", m_trackId);
	}
	else if(m_playbackTake != nullptr)
	{
		((IAudioSource^)m_playbackTake)->AddTarget(m_target);
		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 108, "Trk {0}: Take from {1} to {2}", m_trackId, m_playbackTake->Offset, m_playbackTake->End);
	}
	else
	{
		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 109, "Trk {0}: Waiting for first take", m_trackId);
	}
	return !m_isAtEndOfStream;
}

void AudioTrack::Prepare()
{
	m_playbackIndex = 0;

	while (AdvancePlaybackTake() && m_playbackTake->End <= m_machine->Position);

	if (m_playbackTake != nullptr)
	{
		bool canSetPos = m_playbackTake->TrySetPosition(m_machine->Position);
	}

	NewTake();
}

void AudioTrack::InsertTake(IAudioTake^ take)
{
	if (nullptr == take)
		throw gcnew ArgumentNullException();

	take->Initialize();

	m_takes->Add(take->Offset, take);
}

void AudioTrack::NewTake()
{
	if (IsHot)
	{
		m_recordingTake = gcnew AudioRecording(m_machine->Position, m_machine->Router->SampleRate, m_machine->GetTempAudioFileName());
	}
}

void AudioTrack::CompleteTake()
{
	if (nullptr != m_recordingTake)
	{
		m_recordingTake->Finish();
		IAudioTake^ oldTake = nullptr;

		if (m_takes->TryGetValue(m_recordingTake->Offset, oldTake))
		{
			// TODO: Handle concurrent takes in same position
			m_takes->Remove(oldTake->Offset);			
		}
		m_takes->Add(m_recordingTake->Offset, m_recordingTake);

		m_recordingTake = nullptr;
	}
}

void AudioTrack::AbortTake()
{
	if (nullptr != m_recordingTake)
	{
		delete m_recordingTake;
		m_recordingTake = nullptr;
	}
}

bool AudioTrack::NextFrame()
{
	try
	{
		if (IsHot)
		{
			// target and recording take are linked to source, when hot/recording
			//Source->WriteTo(m_target);

			//if (IsRecording)
			//{
			//	Source->WriteTo(m_recordingTake);
			//}
			//return true;
		}
		else
		{
			int samplesTotal = 0;
			IAudioSource^ source = (IAudioSource^)m_playbackTake;

			while (nullptr != m_playbackTake)
			{
				// TODO: Handle partial reads in a single frame
				if (m_playbackTake->Position <= m_machine->Position)
				{
					source->Read(nullptr);

					if(m_playbackTake->End <= m_machine->Position)
					{
						if (!AdvancePlaybackTake())
						{
							break;
						}
						source = (IAudioSource^)m_playbackTake;
					}
				}
				else
				{
					break;
				}
			}
			return !m_isAtEndOfStream;
		}
	}
	catch (System::IO::IOException^)
	{
		EndRecording();
		m_isAtEndOfStream = true;

		throw;
	}
}

void AudioTrack::Send()
{
}

String^ AudioTrack::ToString()
{
	return String::Format("{0} ({1}/{2}/{3})", Name, IsHot ? "H" : "-", IsMuted ? "M" : "-", IsSolo ? "S" : "-");
}

