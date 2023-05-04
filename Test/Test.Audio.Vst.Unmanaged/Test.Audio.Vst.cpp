#include "pch.h"
#include "CppUnitTest.h"
#include "Constants.h"
#include <VstObjectFactory.h>
#include <MemCheck.h>

using namespace Test::Audio::Common;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Vst::Unmanaged;
using namespace Test::Audio::Vst::Unmanaged;

namespace TestAudioVst
{
	TEST_CLASS(TestAudioVst)
	{
	public:		
		TEST_METHOD(CreateAudioProcessor)
		{
			const wchar_t* pwcszLibName = L"C:\\Program Files\\Common Files\\VST3\\Unfiltered Audio Indent.vst3";

			IVstHostPtr host = VstObjectFactory::CreateVstHost(L"Ludger", Constants::SampleCount, Constants::SampleRate);

			std::wstring pluginId = host->AddLibrary(pwcszLibName);

			Assert::IsNotNull(pluginId.c_str(), L"Can load plugin library.");

			ISampleProcessorPtr audioProcessor = host->CreateSampleProcessor(pluginId.c_str());

			Assert::IsNotNull(audioProcessor.GetInterfacePtr(), L"Can create ISampleProcessor.");
		}

		TEST_METHOD_INITIALIZE(Init)
		{
			m_memCheck.BeginCheck();
		}

		TEST_METHOD_CLEANUP(CleanUp)
		{
			m_memCheck.EndCheck();
		}
	private:
		MemCheck m_memCheck;
	};
}
