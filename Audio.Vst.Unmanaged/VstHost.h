#pragma once
#include <Audio.Vst.Unmanaged.h>
#include "IVstHost.h"
#include "PluginLibrary.h"
#include <UnknownBase.h>
#include <vector>

using namespace Audio::Vst::Unmanaged::Abstractions;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class VstHost : public IVstHost
			{
			public:
				VstHost(IHostApplicationPtr& application, int sampleCount, double sampleRate);
				virtual ~VstHost();

				const wchar_t* AddLibrary(const wchar_t* pwcszPath);

				void RemoveAllLibraries();

				ISampleProcessorPtr CreateSampleProcessor(const wchar_t* pwcszPluginId);

				DECLARE_IUNKNOWN

			private:
				PluginLibraryPtr FindPlugin(const wchar_t* pwcszPluginId);

				std::vector<PluginLibraryPtr> m_libraries;
				IHostApplicationPtr m_application;
				int m_sampleCount;
				double m_sampleRate;
			};
		}
	}
}