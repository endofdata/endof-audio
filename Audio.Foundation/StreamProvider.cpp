#include "pch.h"
#include "StreamProvider.h"

using namespace Audio::Foundation;

StreamProvider::StreamProvider(int sampleCount, System::IO::Stream^ wavStream) :
	SampleContainer(sampleCount),
	m_stream(wavStream)
{
}
