#include "pch.h"
#include "ManagedLooperTrack.h"

using namespace System;
using namespace Audio::Asio::Interop;
using namespace Audio::Foundation::Unmanaged;

ManagedLooperTrack::ManagedLooperTrack(Guid id, MixParameter& parameter) : _id(id), _parameter(parameter)
{
}


double ManagedLooperTrack::Level::get()
{
	return _parameter.Level;
}

void ManagedLooperTrack::Level::set(double value)
{
	_parameter.Level = value;
}

double ManagedLooperTrack::Pan::get()
{
	return _parameter.Pan;
}

void ManagedLooperTrack::Pan::set(double value)
{
	_parameter.Pan = value;
}
