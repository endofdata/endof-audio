#include "pch.h"
#include "CppUnitTest.h"
#include <OscString.h>
#include <MemCheck.h>

using namespace Audio::Osc::Unmanaged;
using namespace Test::Audio::Common;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestAudioOscUnmanaged
{
	TEST_CLASS(OscStringTests)
	{
	public:
		
		TEST_METHOD(ReadStrings)
		{
			std::stringstream buffer;

			buffer.write("A\0\0\0B2\0\0C3x\0Dull\0\0\0\0Multi segments string\0\0\0", 44);

			OscString a;
			buffer >> a;

			Assert::AreEqual("A", a, L"String with one character.");
			Assert::AreEqual(1, a.Size);
			Assert::AreEqual(4, a.PaddedSize);

			OscString b;
			buffer >> b;

			Assert::AreEqual("B2", b, L"String with two characters.");
			Assert::AreEqual(2, b.Size);
			Assert::AreEqual(4, b.PaddedSize);

			OscString c;
			buffer >> c;

			Assert::AreEqual("C3x", c, L"String with three characters.");
			Assert::AreEqual(3, c.Size);
			Assert::AreEqual(4, c.PaddedSize);

			OscString d;
			buffer >> d;

			Assert::AreEqual("Dull", d, L"String with four characters.");
			Assert::AreEqual(4, d.Size);
			Assert::AreEqual(8, d.PaddedSize);

			OscString multi;
			buffer >> multi;	

			Assert::AreEqual("Multi segments string", multi, L"String with many characters and whitespace.");
			Assert::AreEqual(21, multi.Size);
			Assert::AreEqual(24, multi.PaddedSize);
		}

		TEST_METHOD(WriteStrings)
		{
			std::stringstream buffer;

			buffer << OscString("A");
			buffer << OscString("B2");
			buffer << OscString("C3x");
			buffer << OscString("Dull");
			buffer << OscString("Multi segments string");

			char formatted[45];
			std::memset(formatted, 0, sizeof(formatted));
			buffer.read(formatted, sizeof(formatted) - 1);

			Assert::AreEqual("A\0\0\0B2\0\0C3x\0Dull\0\0\0\0Multi segments string\0\0\0", formatted, L"Padded writing of strings is ok.");
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
