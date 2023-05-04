#include "pch.h"
#include "CppUnitTest.h"
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
				TEST_CLASS(ContainerReaderTests)
				{
				public:
					TEST_METHOD(ByRefOrByValue)
					{
						ISampleBufferPtr pSampleBuffer = ObjectFactory::CreateSampleBuffer(Constants::SampleCount);

						// This will call AddRef()
						CallWithInstance(pSampleBuffer);

						// The following variants do not call AddRef()
						CallWithRef(pSampleBuffer);
						CallWithConstRef(pSampleBuffer);
						CallWithRawPointer(pSampleBuffer);
					}

					int CallWithInstance(ISampleBufferPtr pRef)
					{
						return pRef->SampleCount;
					}

					int CallWithRef(ISampleBufferPtr& pRef)
					{
						return pRef->SampleCount;
					}

					int CallWithConstRef(const ISampleBufferPtr& pRef)
					{
						return pRef->SampleCount;
					}

					int CallWithRawPointer(ISampleBuffer& pRef)
					{
						return pRef.SampleCount;
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