#include "pch.h"
#include "GuidConversion.h"
#include "Looper.h"
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
	_unmanaged = inner;
}

Looper::~Looper()
{
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

void Looper::IsLooping::set(bool value)
{
	OnPropertyChanged(IsLoopingProperty);
}

float Looper::LoopPosition::get()
{
	return _loopPosition;
}

void Looper::LoopPosition::set(float value)
{
	if (value != _loopPosition)
	{
		_loopPosition = value;
		OnPropertyChanged(LoopPositionProperty);
	}
}

int Looper::LoopLength::get()
{
	return _loopLength;
}

void Looper::LoopLength::set(int value)
{
	if (value != _loopLength)
	{
		_loopLength = value;
		OnPropertyChanged(LoopLengthProperty);
	}
}

int Looper::LoopCount::get()
{
	return _unmanaged->LoopCount;
}

void Looper::LoopCount::set(int value)
{
	OnPropertyChanged(LoopCountProperty);
}

bool Looper::IsLoopRecording::get()
{
	return _unmanaged->IsLoopRecording;
}

void Looper::IsLoopRecording::set(bool value)
{
	OnPropertyChanged(IsLoopRecordingProperty);
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

TransportCode Looper::TransportStatus::get()
{
	return _transportStatus;
}

void Looper::TransportStatus::set(TransportCode value)
{
	if (_transportStatus != value)
	{
		if (_transportStatus != _previousTransportStatus)
		{
			PreviousTransportStatus = _transportStatus;
		}
		_transportStatus = value;
		OnPropertyChanged(TransportStatusProperty);
	}
}

TransportCode Looper::PreviousTransportStatus::get()
{
	return _previousTransportStatus;
}

void Looper::PreviousTransportStatus::set(TransportCode value)
{
	if (value != _previousTransportStatus)
	{
		OnPropertyChanged(PreviousTransportStatusProperty);
	}
}

void Looper::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}


void Looper::OnTransportStatusChanged(TransportCode previous, TransportCode current)
{
	if (previous != _transportStatus)
	{
		TransportStatus = previous;
	}
	TransportStatus = current;

}

void Looper::OnLoopRestart()
{
}