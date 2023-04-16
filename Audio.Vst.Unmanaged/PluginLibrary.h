#pragma once
#include <Audio.Vst.Unmanaged.h>
#include <string>
#include <VstCom.h>
#include <AudioProcessor.h>
#include <memory>
#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class PluginLibrary;
			typedef std::shared_ptr<PluginLibrary> PluginLibraryPtr;

			class PluginLibrary
			{
			public:
				virtual ~PluginLibrary();

				ISampleProcessorPtr CreateAudioProcessor(IHostApplicationPtr& context, int sampleCount, int sampleRate);

				bool get_IsValid() const;
				_declspec(property(get = get_IsValid)) bool IsValid;

				const std::wstring& get_Id() const;
				_declspec(property(get = get_Id)) const std::wstring& Id;

				static PluginLibraryPtr LoadFrom(const wchar_t* pwcszPath);

			private:
				PluginLibrary(HMODULE hLibrary, const std::wstring& id);
				void Initialize();
				void Uninitialize();
				bool CreateFactory();
				bool FindClassInfo(const char* pcszCategory, PClassInfoW& classInfo);

				HMODULE m_handle;
				std::wstring m_id;
				IPluginFactoryPtr m_factory;
				IPluginFactory2Ptr m_factory2;
				IPluginFactory3Ptr m_factory3;
			};
		}
	}
}