#include "pch.h"
#include "FileSource.h"
#include "ObjectFactory.h"

using namespace Audio::Foundation::Unmanaged;


FileSource::FileSource(const std::string& filename, ISampleContainerPtr& container) :
	m_reader(filename, container),
	m_pFirst(nullptr),
	m_refCount(0)
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

ISampleProcessorPtr& FileSource::get_First()
{
	return m_pFirst;
}

void FileSource::put_First(ISampleProcessorPtr& value)
{
	m_pFirst = value;
}

bool FileSource::get_HasFirst()
{
	return m_pFirst != nullptr;
}

ISampleContainerPtr FileSource::get_Container()
{
	return m_reader.Container;
}

void FileSource::OnNextBuffer(bool readSecondHalf)
{
	if (HasFirst)
	{
		m_reader.OnNextBuffer(readSecondHalf);
		ISampleContainerPtr container = get_Container();
		m_pFirst->Process(container);
	}
}

