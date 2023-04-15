#include "pch.h"
#include "PluginLibrary.h"

using namespace Audio::Vst::Unmanaged;

PluginLibrary::PluginLibrary(HMODULE handle) :
	m_handle(handle),
	m_factory(nullptr),
	m_factory2(nullptr),
	m_factory3(nullptr)
{
}

PluginLibrary::~PluginLibrary()
{
	Uninitialize();
}

bool PluginLibrary::get_IsValid() const
{
	return m_handle != nullptr;
}

void PluginLibrary::Initialize()
{
	if (IsValid)
	{
		auto initDll = GetProcAddress(m_handle, "InitDll");

		if (initDll != nullptr)
		{
			initDll();
		}
	}
}

bool PluginLibrary::CreateFactory()
{
	if (IsValid)
	{
		auto getPluginFactory = GetProcAddress(m_handle, "GetPluginFactory");

		if (getPluginFactory != nullptr)
		{
			m_factory = OPtr<IPluginFactory>(reinterpret_cast<IPluginFactory*>(getPluginFactory()));

			if (m_factory == nullptr)
			{
				Uninitialize();
			}
			else
			{
				IPluginFactory2* pFuck2 = nullptr;
				m_factory->queryInterface(IPluginFactory2_iid, reinterpret_cast<void**>(&pFuck2));
				m_factory2 = OPtr<IPluginFactory2>(pFuck2);
				IPluginFactory3* pFuck3 = nullptr;
				m_factory->queryInterface(IPluginFactory3_iid, reinterpret_cast<void**>(&pFuck3));
				m_factory3 = OPtr<IPluginFactory3>(pFuck3);

				return true;
			}
		}
	}
	return false;
}

IAudioProcessorPtr PluginLibrary::CreateAudioProcessor(IHostApplicationPtr& context)
{
	std::string audioModuleCategory = "Audio Module Class";
	PClassInfoW classInfo;

	if (FindClassInfo(audioModuleCategory.c_str(), classInfo))
	{
		IComponent* pCompRaw = nullptr;
		
		if (kResultOk == m_factory->createInstance(classInfo.cid, reinterpret_cast<FIDString>(IComponent_iid), reinterpret_cast<void**>(&pCompRaw)))
		{
			IComponentPtr component(pCompRaw, false);
			IPluginBase* pBaseRaw = nullptr;

			if (kResultOk == component->queryInterface(IPluginBase_iid, reinterpret_cast<void**>(&pBaseRaw)))
			{
				IPluginBasePtr pluginBase(pBaseRaw, false);
				pluginBase->initialize(context);

				IAudioProcessor* pProcRaw = nullptr;

				if (kResultOk == component->queryInterface(reinterpret_cast<FIDString>(IAudioProcessor_iid), reinterpret_cast<void**>(&pProcRaw)))
				{					
					return IAudioProcessorPtr(pProcRaw, false);
				}
			}
		}
	}
	return nullptr;
}

bool PluginLibrary::FindClassInfo(const char* pcszCategory, PClassInfoW& classInfo)
{
	if (IsValid && m_factory == nullptr)
	{
		CreateFactory();
	}

	if (IsValid)
	{
		if (m_factory3 != nullptr)
		{
			int classCount = m_factory3->countClasses();
			PClassInfoW classInfoW;

			for (int c = 0; c < classCount; c++)
			{
				if(!m_factory3->getClassInfoUnicode(c, &classInfo) && !std::strcmp(pcszCategory, classInfo.category))
				{
					return true;
				}
			}
		}
		else if (m_factory2 != nullptr)
		{
			int classCount = m_factory2->countClasses();
			PClassInfo2 classInfo2;
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf16conv;

			for (int c = 0; c < classCount; c++)
			{
				if (!m_factory2->getClassInfo2(c, &classInfo2) && !std::strcmp(pcszCategory, classInfo2.category))
				{
					std::wstring name = utf16conv.from_bytes(classInfo2.name);
					std::wstring vendor = utf16conv.from_bytes(classInfo2.vendor);
					std::wstring version = utf16conv.from_bytes(classInfo2.version);
					std::wstring sdkVersion = utf16conv.from_bytes(classInfo2.sdkVersion);

					classInfo = PClassInfoW(classInfo2.cid, classInfo2.cardinality, classInfo2.category, name.c_str(),
						classInfo2.classFlags, classInfo2.subCategories, vendor.c_str(), version.c_str(), sdkVersion.c_str());
					return true;
				}
			}
		}
		else if (m_factory != nullptr)
		{
			int classCount = m_factory->countClasses();
			PClassInfo classInfo1;
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf16conv;

			for (int c = 0; c < classCount; c++)
			{
				if (!m_factory->getClassInfo(c, &classInfo1) && !std::strcmp(pcszCategory, classInfo1.category))
				{
					std::wstring name = utf16conv.from_bytes(classInfo1.name);

					classInfo = PClassInfoW(classInfo.cid, classInfo.cardinality, classInfo.category, name.c_str(), 0, "", L"", L"", L"");
					return true;
				}
			}
		}
	}
	return false;
}

void PluginLibrary::Uninitialize()
{
	if (IsValid)
	{
		m_factory3.reset();
		m_factory2.reset();
		m_factory.reset();

		auto exitDll = GetProcAddress(m_handle, "ExitDll");

		if (exitDll != nullptr)
		{
			exitDll();
		}
		FreeLibrary(m_handle);
		m_handle = nullptr;
	}
}

PluginLibrary PluginLibrary::LoadFrom(const wchar_t* pwcszPath)
{
	HMODULE hLibrary = LoadLibraryEx(pwcszPath, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

	PluginLibrary result = PluginLibrary(hLibrary);

	if (result.IsValid)
	{
		result.Initialize();
	}
	return result;
}
