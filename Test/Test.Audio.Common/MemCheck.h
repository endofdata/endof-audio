#pragma once
#include <Test.Audio.Common.h>
#include <crtdbg.h>

namespace Test
{
	namespace Audio
	{
		namespace Common
		{
			class _TEST_AUDIO_COMMON_API MemCheck
			{
			public:
				MemCheck();
				virtual ~MemCheck();

				void BeginCheck();

				void EndCheck();

			private:
				_CrtMemState m_preTest;
				_CrtMemState m_postTest;
			};
		}
	}
}