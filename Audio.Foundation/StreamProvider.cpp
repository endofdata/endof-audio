#include "pch.h"
#include "StreamProvider.h"
#include <ObjectFactory.h>

using namespace Audio::Foundation;
using namespace Audio::Foundation::Unmanaged;

StreamProvider::StreamProvider(int sampleCount, System::IO::Stream^ wavStream) : m_stream(wavStream)
{
	m_pContainer = ObjectFactory::CreateSampleContainer(sampleCount);
	m_pSharer = ObjectFactory::CreateSampleSharer();
}

StreamProvider::~StreamProvider()
{
	m_pContainer->Release();
	m_pSharer->Release();
}

ISampleContainer& StreamProvider::get_Container()
{
	return *m_pContainer;
}

ISampleSharer& StreamProvider::get_Sharer()
{
	return *m_pSharer;
}
