#include "pch.h"
#include "AudioTrackCollection.h"
#include "AudioTrack.h"

using namespace System;
using namespace Audio::Asio::Interop;

AudioTrack^ AudioTrackCollection::Find(int trackId)
{
	// TODO Is there a chance to use LINQ with a lambda (including the closure for 'trackId')?
	for each (AudioTrack^ track in this)
	{
		if (track->TrackId == trackId)
			return track;
	}
	return nullptr;
}
