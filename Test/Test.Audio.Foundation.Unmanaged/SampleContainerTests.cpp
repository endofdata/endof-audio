#include "pch.h"
#include "CppUnitTest.h"
#include "Constants.h"
#include "HelperMethods.h"
#include <ISampleBuffer.h>
#include <ObjectFactory.h>


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
						ISampleContainerPtr pContainer = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

					TEST_METHOD(AddAndRemoveChannels)
					{
						ISampleContainerPtr pContainer = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						pContainer->ChannelCount += 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount + 2);

						pContainer->ChannelCount -= 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

					TEST_METHOD(ResizeBuffers)
					{
						ISampleContainerPtr pContainer = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						pContainer->SampleCount *= 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount * 2, Constants::ChannelCount);

						pContainer->SampleCount = Constants::SampleCount;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

				};
			}
		}
	}
}
