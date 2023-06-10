#include "pch.h"
#include "RegisteredMidiInput.h"

using namespace System;
using namespace Audio::Foundation::Interop;

RegisteredMidiInput::RegisteredMidiInput(unsigned int id, String^ name)
{
	_id = id;
	_name = name;
}

unsigned int RegisteredMidiInput::Id::get()
{
	return _id;
}

String^ RegisteredMidiInput::Name::get()
{
	return _name;
}

// virtual 
String^ RegisteredMidiInput::ToString()
{
	return _name;
}

