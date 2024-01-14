#include "pch.h"
#include "CppUnitTest.h"
#include <AsioCore.h>
#include <SteinbergUrRt2.h>
#include <conio.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Asio::Unmanaged;

namespace TestAudioAsioUnmanaged
{
	TEST_CLASS(TestAudioAsioUnmanaged)
	{
	public:
		
		TEST_METHOD(Run30SecondsAsioCoreIO)
		{
			bool isPrioritySet = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

			Assert::IsTrue(isPrioritySet, L"Process priority can be set to HIGH_PRIORITY_CLASS");

			AsioCorePtr asioCore = AsioCore::CreateInstancePtr(IID_STEINBERG_UR_RT2);

			int selectedInputs[] = { 0, 1 };
			int selectedOutputs[] = { 0, 1 };
			int sampleCount = 512;
			float outputSaturation = 1.0f;

			asioCore->CreateBuffers(selectedInputs, _countof(selectedInputs), selectedOutputs, _countof(selectedOutputs), AsioCore::UsePreferredSize, outputSaturation);

			IProcessingChainPtr processingChain = asioCore->ProcessingChain;

			processingChain->OutputChannelPair[0]->IsActive = true;
			processingChain->InputChannel[0]->IsActive = true;

			asioCore->Start();

			Sleep(30000);

			asioCore->Stop();
		}
	};
}
