#include "pch.h"
#include "PluginLibrary.h"
#include "ParameterChanges.h"
#include "StrConv.h"

using namespace Audio::Vst::Unmanaged;

PluginLibrary::PluginLibrary(HMODULE handle, const std::wstring& id) :
	m_handle(handle),
	m_id(id),
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

const std::wstring& PluginLibrary::get_Id() const
{
	return m_id;
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
			m_factory = IPluginFactoryPtr(reinterpret_cast<IPluginFactory*>(getPluginFactory()), false);

			if (m_factory.get() == nullptr)
			{
				Uninitialize();
			}
			else
			{
				IPluginFactory2* pFuck2 = nullptr;
				m_factory->queryInterface(IPluginFactory2_iid, reinterpret_cast<void**>(&pFuck2));
				m_factory2 = IPluginFactory2Ptr(pFuck2, false);

				IPluginFactory3* pFuck3 = nullptr;
				m_factory->queryInterface(IPluginFactory3_iid, reinterpret_cast<void**>(&pFuck3));
				m_factory3 = IPluginFactory3Ptr(pFuck3, false);

				return true;
			}
		}
	}
	return false;
}

ISampleProcessorPtr PluginLibrary::CreateAudioProcessor(IHostApplicationPtr& context, int sampleCount, int sampleRate)
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
					IAudioProcessorPtr processor(pProcRaw, false);
					IParameterChangesPtr paramChanges(new ParameterChanges());

					auto processorWrapper = new AudioProcessor(processor, paramChanges, sampleCount, sampleRate);

					ISampleProcessor* pSmpProcRaw = nullptr;
					processorWrapper->QueryInterface(__uuidof(ISampleProcessor), reinterpret_cast<void**>(&pSmpProcRaw));

					return ISampleProcessorPtr(pSmpProcRaw, false);
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
				if (!m_factory3->getClassInfoUnicode(c, &classInfo) && !std::strcmp(pcszCategory, classInfo.category))
				{
					return true;
				}
			}
		}
		else if (m_factory2 != nullptr)
		{
			int classCount = m_factory2->countClasses();
			PClassInfo2 classInfo2;

			for (int c = 0; c < classCount; c++)
			{
				if (!m_factory2->getClassInfo2(c, &classInfo2) && !std::strcmp(pcszCategory, classInfo2.category))
				{
					std::wstring name = StrConv::ToUtf16(classInfo2.name);
					std::wstring vendor = StrConv::ToUtf16(classInfo2.vendor);
					std::wstring version = StrConv::ToUtf16(classInfo2.version);
					std::wstring sdkVersion = StrConv::ToUtf16(classInfo2.sdkVersion);

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

			for (int c = 0; c < classCount; c++)
			{
				if (!m_factory->getClassInfo(c, &classInfo1) && !std::strcmp(pcszCategory, classInfo1.category))
				{
					std::wstring name = StrConv::ToUtf16(classInfo1.name);

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

PluginLibraryPtr PluginLibrary::LoadFrom(const wchar_t* pwcszPath)
{
	HMODULE hLibrary = LoadLibraryEx(pwcszPath, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	std::filesystem::path path(pwcszPath);
	std::wstring id = path.has_filename() ? path.filename() : L"invalid";

	PluginLibraryPtr result = std::shared_ptr<PluginLibrary>(new PluginLibrary(hLibrary, id), [](PluginLibrary* self)
	{
		if (self != nullptr)
		{
			delete self;
		}
	});

	if (result->IsValid)
	{
		result->Initialize();
	}
	return result;
}
