#include "pch.h"
#include "VstHost.h"

using namespace Audio::Vst::Unmanaged;

VstHost::VstHost(IHostApplicationPtr& application, int sampleCount, double sampleRate) :
	m_application(application),
	m_sampleCount(sampleCount),
	m_sampleRate(sampleRate)
{
}

VstHost::~VstHost()
{
}

IMPLEMENT_IUNKNOWN(VstHost)

bool VstHost::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(IVstHost))
	{
		*ppvResult = dynamic_cast<IVstHost*>(this);
		return true;
	}
	return false;
}

const wchar_t* VstHost::AddLibrary(const wchar_t* pwcszPath)
{
	PluginLibraryPtr library = PluginLibrary::LoadFrom(pwcszPath);

	if (library->IsValid)
	{
		m_libraries.push_back(library);
		return library->Id.c_str();
	}
	return nullptr;
}

ISampleProcessorPtr VstHost::CreateSampleProcessor(const wchar_t* pwcszPluginId)
{
	PluginLibraryPtr plugin = FindPlugin(pwcszPluginId);

	if (plugin == nullptr)
	{
		return nullptr;
	}
	return plugin->CreateAudioProcessor(m_application, m_sampleCount, m_sampleRate);
}

PluginLibraryPtr VstHost::FindPlugin(const wchar_t* pwcszPluginId)
{
	for (auto&& plugin : m_libraries)
	{
		if (plugin->Id.compare(pwcszPluginId) == 0)
		{
			return plugin;
		}
	}
	return nullptr;
}

