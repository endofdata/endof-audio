#pragma once
#include <Audio.Vst.Unmanaged.h>
#include <string>
#include <VstCom.h>
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
			class _AUDIO_VST_UNMANAGED_API PluginLibrary
			{
			public:
				virtual ~PluginLibrary();

				IAudioProcessorPtr CreateAudioProcessor(IHostApplicationPtr& context);

				bool get_IsValid() const;
				_declspec(property(get = get_IsValid)) bool IsValid;

				static PluginLibrary LoadFrom(const wchar_t* pwcszPath);

			private:
				PluginLibrary(HMODULE hLibrary);
				void Initialize();
				void Uninitialize();
				bool CreateFactory();
				bool FindClassInfo(const char* pcszCategory, PClassInfoW& classInfo);

				HMODULE m_handle;
				IPluginFactoryPtr m_factory;
				IPluginFactory2Ptr m_factory2;
				IPluginFactory3Ptr m_factory3;
			};
		}
	}
}