#include "pch.h"
#include "GuidConversion.h"
#include "ManagedLooper.h"
#include "LooperEvents.h"
#include <AsioObjectFactory.h>

using namespace System;

using namespace Audio::Asio::Interop;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Asio::Unmanaged::Abstractions;
using namespace Audio::Foundation::Interop;

ManagedLooper^ ManagedLooper::Create(ManagedLooperConfig^ config)
{
	ILooperConfigPtr unmanagedConfig = AsioObjectFactory::CreateLooperConfiguration();

	pin_ptr<const wchar_t> pName = PtrToStringChars(config->Name);

	unmanagedConfig->Name = pName;
	unmanagedConfig->MidiInput = config->MidiInput;
	unmanagedConfig->AsioDevice = GuidConversion::FromManaged(config->AsioDevice);

	for each (int id in config->InputChannelList)
	{
		unmanagedConfig->AddInputChannel(id);
	}

	for each (int id in config->OutputChannelList)
	{
		unmanagedConfig->AddOutputChannel(id);
	}

	unmanagedConfig->SampleCount = config->SampleCount;
	unmanagedConfig->OutputSaturation = config->OutputSaturation;

	ILooper* unmanagedLooper = AsioObjectFactory::CreateLooper(unmanagedConfig).Detach();

	return gcnew ManagedLooper(unmanagedLooper);
}

ManagedLooper::ManagedLooper(ILooper* inner)
{
	if (inner == nullptr)
	{
		throw gcnew ArgumentNullException("inner");
	}
	inner->AddRef();

	_events = new LooperEvents(this);
	_events->AddRef();
	inner->LooperEvents = ILooperEventsPtr(_events);

	_unmanaged = inner;
}

ManagedLooper::~ManagedLooper()
{
	_events->Release();
	_unmanaged->Release();
}

bool ManagedLooper::SelectInput(int input, bool isSelected)
{
	return _unmanaged->SelectInput(input, isSelected);
}

bool ManagedLooper::SelectOutputPair(int left, int right, bool isSelected)
{
	int pair[] = { left, right };

	return _unmanaged->SelectOutputPair(pair, isSelected);
}

void ManagedLooper::Run()
{
	_unmanaged->Run();
}

void ManagedLooper::SaveSession(String^ filenameBase)
{
	pin_ptr<const wchar_t> wchars = PtrToStringChars(filenameBase);

	_unmanaged->SaveSession(wchars);
}

String^ ManagedLooper::AddVstPlugin(String^ vstLibrary)
{
	pin_ptr<const wchar_t> wchars = PtrToStringChars(vstLibrary);

	const wchar_t* pluginId = _unmanaged->AddVstPlugin(wchars);

	return gcnew String(pluginId);
}

int ManagedLooper::InsertFx(String^ pluginId)
{
	pin_ptr<const wchar_t> wchars = PtrToStringChars(pluginId);

	return _unmanaged->InsertFx(wchars);
}

//int InsertFx(ISampleProcessorPtr effect);
//bool RemoveFx(int id);

bool ManagedLooper::IsLooping::get()
{
	return _unmanaged->IsLooping;
}

float ManagedLooper::LoopPosition::get()
{
	return _loopPosition;
}

int ManagedLooper::LoopLength::get()
{
	return _loopLength;
}

int ManagedLooper::LoopCount::get()
{
	return _unmanaged->LoopCount;
}

ManagedAudioTime^ ManagedLooper::TransportPosition::get()
{
	return _transportPosition;
}

void ManagedLooper::TransportPosition::set(ManagedAudioTime^ value)
{
	_transportPosition = value;
	OnPropertyChanged(TransportPositionProperty);
}

RecordingMode ManagedLooper::RecordingStatus::get()
{
	switch (_unmanaged->RecordingStatus)
	{
	case RecordingStatusType::Off:
		return RecordingMode::Off;
	case RecordingStatusType::Armed:
		return RecordingMode::Armed;
	case RecordingStatusType::Recording:
		return RecordingMode::Recording;
	case RecordingStatusType::Unarmed:
		return RecordingMode::Unarmed;
	default:
		throw gcnew NotImplementedException("Unknown RecordingStatusType.");
	}
}

bool ManagedLooper::IsSessionRecording::get()
{
	return _unmanaged->IsSessionRecording;
}

void ManagedLooper::IsSessionRecording::set(bool value)
{
	if (value != _unmanaged->IsSessionRecording)
	{
		_unmanaged->IsSessionRecording = value;
		OnPropertyChanged(IsSessionRecordingProperty);
	}
}

String^ ManagedLooper::Name::get()
{
	return gcnew String(_unmanaged->Name);
}

void ManagedLooper::Name::set(String^ value)
{
	if (value != nullptr)
	{
		pin_ptr<const wchar_t> wchars = PtrToStringChars(value);
		_unmanaged->Name = wchars;
	}
	else
	{
		_unmanaged->Name = nullptr;
	}
	OnPropertyChanged(NameProperty);
}

void ManagedLooper::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}
