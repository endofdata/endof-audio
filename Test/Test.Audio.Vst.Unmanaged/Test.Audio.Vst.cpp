#include "pch.h"
#include "CppUnitTest.h"
#include <PluginLibrary.h>
#include <VstHostApplication.h>
#include <VstCom.h>
#include "Constants.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Vst::Unmanaged;
using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace Test::Audio::Vst::Unmanaged;

namespace TestAudioVst
{
	TEST_CLASS(TestAudioVst)
	{
	public:		
		TEST_METHOD(CreateAudioProcessor)
		{
			const wchar_t* pwcszLibName = L"C:\\Program Files\\Common Files\\VST3\\Unfiltered Audio Indent.vst3";

			PluginLibrary library = PluginLibrary::LoadFrom(pwcszLibName);

			Assert::IsTrue(library.IsValid, L"Can load plugin library.");

			IHostApplicationPtr application = new VstHostApplication(L"Ludger");

			ISampleProcessorPtr audioProcessor = library.CreateAudioProcessor(application, Constants::SampleCount, Constants::SampleRate);

			Assert::IsNotNull(audioProcessor.GetInterfacePtr(), L"Can create ISampleProcessor.");
		}
	};
}
