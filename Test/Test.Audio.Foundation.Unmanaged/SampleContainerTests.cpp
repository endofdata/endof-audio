#include "pch.h"
#include "CppUnitTest.h"
#include "Constants.h"
#include "HelperMethods.h"
#include <ISampleBuffer.h>
#include <FoundationObjectFactory.h>
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
				TEST_CLASS(SampleContainerTests)
				{
				public:
					TEST_METHOD(BasicInitialization)
					{
						ISampleContainerPtr pContainer = FoundationObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

					TEST_METHOD(AddAndRemoveChannels)
					{
						ISampleContainerPtr pContainer = FoundationObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						pContainer->ChannelCount += 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount + 2);

						pContainer->ChannelCount -= 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

					TEST_METHOD(ResizeBuffers)
					{
						ISampleContainerPtr pContainer = FoundationObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						pContainer->SampleCount *= 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount * 2, Constants::ChannelCount);

						pContainer->SampleCount = Constants::SampleCount;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
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
