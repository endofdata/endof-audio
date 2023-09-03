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

			Assert::AreEqual("A", a, "String with one character.");

			OscString b;
			buffer >> b;

			Assert::AreEqual("B2", b, "String with two characters.");

			OscString c;
			buffer >> c;

			Assert::AreEqual("C3x", c, "String with three characters.");

			OscString d;
			buffer >> d;

			Assert::AreEqual("Dull", d, "String with four characters.");

			OscString multi;
			buffer >> multi;	

			Assert::AreEqual("Multi segments string", multi, "String with many characters and whitespace.");
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
