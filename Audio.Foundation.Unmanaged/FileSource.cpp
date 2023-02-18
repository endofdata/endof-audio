#include "pch.h"
#include "FileSource.h"
#include "ObjectFactory.h"

using namespace Audio::Foundation::Unmanaged;


FileSource::FileSource(const std::string& filename, ISampleContainerPtr container) :
	m_reader(filename),
	m_refCount(0),
	m_pContainer(container)
{
}

FileSource::~FileSource()
{
}

IMPLEMENT_IUNKNOWN(FileSource)

bool FileSource::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleProcessor*>(this));
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}


const std::string& FileSource::get_Filename() const
{
	return m_reader.Filename;
}

int FileSource::get_SampleType()
{
	return 0;
}

ISampleProcessorPtr FileSource::get_First()
{
	return m_pFirst;
}

void FileSource::put_First(ISampleProcessorPtr value)
{
	m_pFirst = value;
}

bool FileSource::get_HasFirst()
{
	return m_pFirst != nullptr;
}

bool FileSource::get_IsActive()
{
	return (bool)m_isActive;
}

void FileSource::put_IsActive(bool value)
{
	m_isActive = value;
}

bool FileSource::get_SupportsDirectMonitor()
{
	return false;
}

IOutputChannelPairPtr FileSource::get_DirectMonitor()
{
	return nullptr;
}

void FileSource::put_DirectMonitor(IOutputChannelPairPtr value)
{
}

bool FileSource::get_HasDirectMonitor()
{
	return false;
}

void FileSource::OnNextBuffer(bool readSecondHalf)
{
	if (IsActive && HasFirst)
	{
		m_reader.Read(m_pContainer);
		m_pFirst->Process(m_pContainer);
	}
}

