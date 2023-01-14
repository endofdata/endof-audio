#include "pch.h"
#include "FileSource.h"
#include "ObjectFactory.h"

using namespace Audio::Foundation::Unmanaged;


FileSource::FileSource(const std::string& filename, ISampleContainerPtr container) :
	m_reader(filename),
	m_container(container),
	m_sharer(NULL),
	m_isActive(false),
	m_refCount(0)
{
	m_sharer = ObjectFactory::CreateSampleSharer();
	m_sharer->Source = m_container;
}

FileSource::~FileSource()
{
}

IMPLEMENT_IUNKNOWN(FileSource)

bool FileSource::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleSource*>(this));
		return true;
	}
	if (iid == __uuidof(ISampleSource))
	{
		*ppvResult = dynamic_cast<ISampleSource*>(this);
		return true;
	}
	return false;
}


const std::string& FileSource::get_Filename() const
{
	return m_reader.Filename;
}


bool FileSource::get_IsActive()
{
	return m_isActive;
}

void FileSource::put_IsActive(bool value)
{
	m_isActive = value;
}


ISampleSharerPtr FileSource::get_SampleSharer()
{
	return m_sharer;
}


void FileSource::Pull(bool readSecondHalf)
{
	m_reader.Read(m_container);
}


void FileSource::Push()
{
	m_sharer->Push();
}
