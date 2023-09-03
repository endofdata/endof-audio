#include "pch.h"
#include "CppUnitTest.h"
#include <MessageBuilder.h>
#include <MemCheck.h>

using namespace Audio::Osc::Unmanaged;
using namespace Test::Audio::Common;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestAudioOscUnmanaged
{
	TEST_CLASS(MessageBuilderTests)
	{
	public:

		TEST_METHOD(CreateEmptyMessage)
		{
			static const char* address = "/some/test/address";

			MessageBuilder builder;

			builder.AddressPattern = address;

			Assert::AreEqual(24, builder.Size);

			std::stringstream buffer;

			buffer << builder;

			Assert::IsTrue(std::memcmp("/some/test/address\0\0,\0\0\0", buffer.str().c_str(), builder.Size) == 0);
		}


		TEST_METHOD(CreateMessageWithParameters)
		{
			std::string address("/some/test/address");

			MessageBuilder builder;

			builder.AddressPattern = address.c_str();

			TypeTag parameterTypes[] =
			{
				TypeTag::Int32,
				TypeTag::Float32,
				TypeTag::String,
				TypeTag::True
			};

			int par1 = 42;
			float par2 = 0.12;
			std::string par3 = "Something wonderful may happen";

			int varSizes[] = { par3.length() };

			builder.AllocParameters(parameterTypes, _countof(parameterTypes), varSizes, _countof(varSizes));

			Assert::AreEqual(static_cast<int>(
				OscString::GetPaddedStringSize(address.length()) + 
				OscString::GetPaddedStringSize(_countof(parameterTypes) + 1) + 
				sizeof(int) + 
				sizeof(float) + 
				OscString::GetPaddedStringSize(par3.length()) + 
				0), 
				builder.Size);

			builder.SetNextParameter(TypeTag::Int32, reinterpret_cast<const void*>(&par1));
			builder.SetNextParameter(TypeTag::Float32, reinterpret_cast<const void*>(&par2));
			builder.SetNextParameter(TypeTag::String, reinterpret_cast<const void*>(par3.c_str()));
			builder.SetNextParameter(TypeTag::True, nullptr);

			std::stringstream buffer;

			buffer << builder;

			const char* expectedResult = "/some/test/address\0\0,ifsT\0\0\0\x2A\0\0\0\x8f\xc2\xf5\x3d\Something wonderful may happen\0\0";

			Assert::IsTrue(std::memcmp(expectedResult, buffer.str().c_str(), builder.Size) == 0);
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
