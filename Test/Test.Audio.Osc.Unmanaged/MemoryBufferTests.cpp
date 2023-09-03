#include "pch.h"
#include "CppUnitTest.h"
#include <MemoryBuffer.h>
#include <MemCheck.h>

using namespace Audio::Osc::Unmanaged;
using namespace Test::Audio::Common;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestAudioOscUnmanaged
{
	TEST_CLASS(MemoryBufferTests)
	{
	public:

		TEST_METHOD(ReadFromStream)
		{
			std::stringstream stream;

			stream.write("There are \0\0\0 empty bytes, \0 in the middle of \0 the stream.", 59);

			MemoryBuffer buffer(stream, 42);
			std::istream istr(&buffer);

			char c;
			istr >> c;
			Assert::AreEqual('T', c, L"First character is 'T'.");

			char tooBig[50];
			std::memset(tooBig, 0, sizeof(tooBig));

			istr.read(tooBig, sizeof(tooBig));

			Assert::AreEqual(true, istr.eof(), L"Is at end of stream.");
			Assert::AreEqual("here are \0\0\0 empty bytes, \0 in the middle", tooBig, L"Can read up to end of buffer.");
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
