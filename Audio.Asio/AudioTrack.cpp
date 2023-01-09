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

	int iFrameSize = (int)(machine->Router->SampleCount);
	m_pCurrentFrame = gcnew array<float>(iFrameSize);

	if (nullptr == m_pCurrentFrame)
		throw gcnew OutOfMemoryException();

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

IAudioInput^ AudioTrack::RecordIn::get()
{
	return m_recordIn;
}

void AudioTrack::RecordIn::set(IAudioInput^ value)
{
	if (value != m_recordIn)
	{
		if (nullptr == value)
		{
			IsHot = false;
		}

		m_recordIn = value;

		OnPropertyChanged(RecordInProperty);
		OnPropertyChanged(IsReadyProperty);

		if (nullptr == value)
		{
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 101, "Trk {0}: RecordIn = null", m_trackId);
		}
		else
		{
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 101, "Trk {0}: RecordIn = channel {1}", m_trackId, value->ChannelId);
		}
	}
}

IAudioOutput^ AudioTrack::MonitorOut::get()
{
	return m_monitorOut;
}

void AudioTrack::MonitorOut::set(IAudioOutput^ value)
{
	if (value != m_monitorOut)
	{
		bool wasMonitoring = false;

		if (m_monitorOut != nullptr)
		{
			wasMonitoring = m_recordIn->RemoveTarget(m_monitorOut);
		}
		m_monitorOut = value;

		OnPropertyChanged(MonitorOutProperty);

		if (nullptr == value)
		{
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 102, "Trk {0}: MonitorOut = null", m_trackId);
		}
		else
		{
			TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 102, "Trk {0}: MonitorOut = channel pair {1}", m_trackId, value->ChannelId);

			if (wasMonitoring)
			{
				m_recordIn->AddTarget(m_monitorOut);
			}
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
	return m_recordIn != nullptr;
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
			m_recordIn->IsActive = true;
			
			if (m_monitorOut != nullptr)
			{
				m_recordIn->AddTarget(m_monitorOut);
			}
			Prepare();
		}
		else
		{
			AbortTake();

			if (IsReady)
			{
				m_recordIn->IsActive = false;
				if (m_monitorOut != nullptr)
				{
					m_recordIn->RemoveTarget(m_monitorOut);
				}
			}
			m_isHot = value;

			EndRecording();

			Array::Clear(m_pCurrentFrame);
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

void AudioTrack::InsertTake(IAudioTake^ take)
{
	if (nullptr == take)
		throw gcnew ArgumentNullException();

	take->Initialize();

	m_takes->Add(take->Offset, take);
}

bool AudioTrack::AdvancePlaybackTake()
{
	m_playbackTake = m_playbackIndex < m_takes->Count ? m_playbackTake = Enumerable::ElementAtOrDefault(m_takes->Values, m_playbackIndex++) : nullptr;
	m_isAtEndOfStream = nullptr == m_playbackTake && m_playbackIndex >= m_takes->Count;

	if (m_isAtEndOfStream)
	{
		TapeMachine::TraceSource->TraceEvent(TraceEventType::Information, 107, "Trk {0}: End of stream", m_trackId);
	}
	else if(m_playbackTake != nullptr)
	{
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
			// TODO: Implement 'CopyCurrentFrame' w/o marshalling?
			//RecordIn->ReadCurrentFrame(m_pCurrentFrame);

			if (IsRecording)
			{
				m_recordingTake->WriteNextFrame(m_pCurrentFrame);
			}
			return true;
		}
		else
		{
			int samplesTotal = 0;

			while (nullptr != m_playbackTake)
			{
				// TODO: Handle partial reads in a single frame
				if (m_playbackTake->Position <= m_machine->Position)
				{
					int samplesRead = m_playbackTake->ReadNextFrame(m_pCurrentFrame);

					if (0 >= samplesRead)
					{
						AdvancePlaybackTake();						
					}
					else
					{
						samplesTotal += samplesRead;

						if (samplesTotal >= m_pCurrentFrame->Length)
						{
							break;
						}
					}
				}
				else
				{
					break;
				}
			}

			int samplesToClear = m_pCurrentFrame->Length - samplesTotal;

			if (samplesToClear > 0)
			{
				Array::Clear(m_pCurrentFrame, samplesTotal, samplesToClear);
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
	//if (nullptr != MonitorOut && !IsSilent)
	//	MonitorOut->WriteCurrentFrame(m_pCurrentFrame, Level, Pan);
}

String^ AudioTrack::ToString()
{
	return String::Format("{0} ({1}/{2}/{3})", Name, IsHot ? "H" : "-", IsMuted ? "M" : "-", IsSolo ? "S" : "-");
}

