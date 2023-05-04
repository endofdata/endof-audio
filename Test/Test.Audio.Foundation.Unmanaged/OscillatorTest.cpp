#include "pch.h"
#include "CppUnitTest.h"
#include "HelperMethods.h"
#include "Constants.h"
#include <ISampleBuffer.h>
#include <ObjectFactory.h>
#include <MemCheck.h>

using namespace Test::Audio::Common;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Foundation::Unmanaged;

namespace Test
{
	namespace Audio
	{
		namespace Foundation
		{
			namespace Unmanaged
			{
				TEST_CLASS(OscillatorTests)
				{
				public:

					TEST_METHOD(BasicInitialization)
					{
						wchar_t tempPath[MAX_PATH];
						std::memset(tempPath, 0, sizeof(tempPath));
						GetTempPath(_countof(tempPath), tempPath);

						wchar_t tempFileName[MAX_PATH];
						std::memset(tempFileName, 0, sizeof(tempFileName));
						UINT unique = (UINT)rand();
						GetTempFileName(tempPath, L"Torsten", unique, tempFileName);

						ISampleProcessorPtr toFileProcessor = ObjectFactory::CreateToFileProcessor(tempFileName);

						ISampleProcessorPtr osc = ObjectFactory::CreateTestOscillator(Constants::SampleRate, 440, 0.5);
						ISampleContainerPtr buffer = ObjectFactory::CreateSampleContainer(Constants::SampleCount, 1);
						ProcessingContext context(Constants::SampleCount);

						for (int b = 0; b < 1000; b++)
						{
							osc->Process(buffer, context);
							toFileProcessor->Process(buffer, context);
							context.SamplePosition += Constants::SampleCount;
						}
						Logger::WriteMessage("File name: ");
						Logger::WriteMessage(tempFileName);
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
		}
	}
}