#include "pch.h"
#include "ToStringImpl.h"
#include "CppUnitTest.h"
#include "Constants.h"
#include <ITake.h>
#include <FoundationObjectFactory.h>


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
				TEST_CLASS(IteratorTests)
				{
				public:

					TEST_METHOD(AssignAndCompare)
					{
						int numTakes = 3;
						auto takes = std::vector<ITakePtr>(numTakes);
						takes.reserve(numTakes);
						auto currentTake = takes.begin();

						int sampleCount = Constants::SampleRate * 5;
						AudioTime takeLength = static_cast<long long>(sampleCount / Constants::SampleRate * 1000.0);
						AudioTime takePosition = 0;

						for (auto iter = takes.begin(); iter!= takes.end(); iter++)
						{
							ISampleContainerPtr container = FoundationObjectFactory::CreateSampleContainer(sampleCount, Constants::ChannelCount);
							ITakePtr take = FoundationObjectFactory::CreateTake(container, takePosition, takeLength);
							*iter = take;
							takePosition += takeLength;
						}

						Assert::IsTrue(currentTake == takes.begin(), L"std::vector<T>::iterator::begin() does not change on std::vector::push_back()");

						auto matches = std::find_if(takes.begin(), takes.end(), [takeLength](ITakePtr& take) { return take->HasDataAt(takeLength); });

						Assert::IsFalse(takes.end() == matches, L"Can find take with data at <takePosition>");

						Assert::IsFalse(matches == currentTake, L"Can compare selected take with std::vector<T>::iterator::begin()");
					}
				};
			}
		}
	}
}
