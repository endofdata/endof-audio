#include "pch.h"
#include "MidiInputRegistry.h"
#include <FoundationObjectFactory.h>

using namespace System;
using namespace System::Collections::ObjectModel;
using namespace Audio::Foundation::Interop;
using namespace System::Runtime::InteropServices;


MidiInputRegistry::MidiInputRegistry()
{
}

unsigned int MidiInputRegistry::GetKeyForItem(RegisteredMidiInput^ item)
{
	return item->Id;
}

MidiInputRegistry^ MidiInputRegistry::FromMmSystem()
{
	MidiInputRegistry^ result = gcnew MidiInputRegistry();

	GCHandle resultHandle = GCHandle::Alloc(result);

	FoundationObjectFactory::SelectMidiInputDevice(
		reinterpret_cast<Audio::Foundation::Unmanaged::Abstractions::MidiInCapsHandler>(MidiInCapsHandler), GCHandle::ToIntPtr(resultHandle).ToPointer());

	resultHandle.Free();

	return result;
}

bool MidiInputRegistry::MidiInCapsHandler(unsigned int deviceId, const MIDIINCAPS& caps, void* callbackParam)
{
	GCHandle resultHandle = GCHandle::FromIntPtr(IntPtr(callbackParam));

	MidiInputRegistry^ result = (MidiInputRegistry^)resultHandle.Target;

	result->Add(gcnew RegisteredMidiInput(deviceId, gcnew String(caps.szPname)));

	return false;
}
