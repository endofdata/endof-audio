#include "pch.h"
#include "CppUnitTest.h"
#include "HelperMethods.h"
#include "Constants.h"
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
				TEST_CLASS(SampleBufferTests)
				{
				public:

					TEST_METHOD(BasicInitialization)
					{
						ISampleBufferPtr pBuffer = ObjectFactory::CreateSampleBuffer(Constants::SampleCount);

						HelperMethods::TestSampleBuffer(pBuffer, Constants::SampleCount);
					}
				};
			}
		}
	}
}