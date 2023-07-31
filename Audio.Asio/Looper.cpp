#include "pch.h"
#include "GuidConversion.h"
#include "Looper.h"
#include "LooperEvents.h"
#include <AsioObjectFactory.h>

using namespace System;

using namespace Audio::Asio::Interop;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Asio::Unmanaged::Abstractions;


Looper^ Looper::Create(LooperConfig^ config)
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

	return gcnew Looper(unmanagedLooper);
}

Looper::Looper(ILooper* inner)
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

Looper::~Looper()
{
	_events->Release();
	_unmanaged->Release();
}

bool Looper::SelectInput(int input, bool isSelected)
{
	return _unmanaged->SelectInput(input, isSelected);
}

bool Looper::SelectOutputPair(int left, int right, bool isSelected)
{
	int pair[] = { left, right };

	return _unmanaged->SelectOutputPair(pair, isSelected);
}

void Looper::Run()
{
	_unmanaged->Run();
}

void Looper::SaveSession(String^ filenameBase)
{
	pin_ptr<const wchar_t> wchars = PtrToStringChars(filenameBase);

	_unmanaged->SaveSession(wchars);
}

String^ Looper::AddVstPlugin(String^ vstLibrary)
{
	pin_ptr<const wchar_t> wchars = PtrToStringChars(vstLibrary);

	const wchar_t* pluginId = _unmanaged->AddVstPlugin(wchars);

	return gcnew String(pluginId);
}

int Looper::InsertFx(String^ pluginId)
{
	pin_ptr<const wchar_t> wchars = PtrToStringChars(pluginId);

	return _unmanaged->InsertFx(wchars);
}

//int InsertFx(ISampleProcessorPtr effect);
//bool RemoveFx(int id);

bool Looper::IsLooping::get()
{
	return _unmanaged->IsLooping;
}

float Looper::LoopPosition::get()
{
	return _loopPosition;
}

int Looper::LoopLength::get()
{
	return _loopLength;
}

int Looper::LoopCount::get()
{
	return _unmanaged->LoopCount;
}

RecordingMode Looper::RecordingStatus::get()
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

bool Looper::IsSessionRecording::get()
{
	return _unmanaged->IsSessionRecording;
}

void Looper::IsSessionRecording::set(bool value)
{
	if (value != _unmanaged->IsSessionRecording)
	{
		_unmanaged->IsSessionRecording = value;
		OnPropertyChanged(IsSessionRecordingProperty);
	}
}

String^ Looper::Name::get()
{
	return gcnew String(_unmanaged->Name);
}

void Looper::Name::set(String^ value)
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

void Looper::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}
