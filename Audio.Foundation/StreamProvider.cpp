#include "pch.h"
#include "StreamProvider.h"
#include <FoundationObjectFactory.h>

using namespace Audio::Foundation;
using namespace Audio::Foundation::Unmanaged;

StreamProvider::StreamProvider(int sampleCount, System::IO::Stream^ wavStream) : m_stream(wavStream)
{
	m_pContainer = FoundationObjectFactory::CreateSampleContainer(sampleCount, 2);
	m_pSharer = FoundationObjectFactory::CreateSampleSharer();
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
