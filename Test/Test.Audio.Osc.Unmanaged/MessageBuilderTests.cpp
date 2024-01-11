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

			Assert::AreEqual(24, builder.Size, L"Message size is ok.");

			std::stringstream buffer;

			buffer << builder;

			Assert::IsTrue(std::memcmp("/some/test/address\0\0,\0\0\0", buffer.str().c_str(), builder.Size) == 0, L"Formatted message is ok.");
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
			float par2 = 0.12f;
			std::string par3 = "Something wonderful may happen";

			int varSizes[] = { static_cast<int>(par3.length()) };

			builder.AllocParameters(parameterTypes, _countof(parameterTypes), varSizes, _countof(varSizes));

			Assert::AreEqual(static_cast<int>(
				OscString::GetPaddedStringSize(static_cast<int>(address.length())) + 
				OscString::GetPaddedStringSize(_countof(parameterTypes) + 1) + 
				sizeof(int) + 
				sizeof(float) + 
				OscString::GetPaddedStringSize(static_cast<int>(par3.length())) + 
				0), 
				builder.Size, L"Size of formatted message is ok.");

			builder.SetNextParameter(TypeTag::Int32, reinterpret_cast<const void*>(&par1));
			builder.SetNextParameter(TypeTag::Float32, reinterpret_cast<const void*>(&par2));
			builder.SetNextParameter(TypeTag::String, reinterpret_cast<const void*>(par3.c_str()));
			builder.SetNextParameter(TypeTag::True, nullptr);

			std::stringstream buffer;

			buffer << builder;

			const char* expectedResult = "/some/test/address\0\0,ifsT\0\0\0\x2A\0\0\0\x8f\xc2\xf5\x3dSomething wonderful may happen\0\0";

			Assert::IsTrue(std::memcmp(expectedResult, buffer.str().c_str(), builder.Size) == 0, L"Formatted message is ok.");
		}

		TEST_METHOD(CreateFromStream)
		{
			std::stringstream source;

			TypeTag parameterTypes[]
			{
				TypeTag::Int32,
				TypeTag::Float32,
				TypeTag::String,
				TypeTag::True
			};

			source.write("/some/test/address\0\0,ifsT\0\0\0\x2A\0\0\0\x8f\xc2\xf5\x3dSomething wonderful may happen\0\0", 68);

			auto builder = MessageBuilder::Create(source);

			Assert::AreEqual("/some/test/address", builder->AddressPattern, L"Address pattern is ok.");
			Assert::AreEqual(static_cast<int>(_countof(parameterTypes)), builder->ParameterCount, L"Parameter count is ok.");

			for (int i = 0; i < builder->ParameterCount; i++)
			{
				Assert::AreEqual(static_cast<char>(parameterTypes[i]), static_cast<char>(builder->ParameterType[i]), L"Parameter types are ok.");

				TypeTag paramType;
				const void* paramData;
				int paramSize;

				builder->GetNextParameter(paramType, paramData, paramSize);

				if (MessageBuilder::IsVariantSize(paramType))
				{
					Assert::AreEqual(static_cast<char>(TypeTag::String), static_cast<char>(paramType), L"This is the string parameter.");
					Assert::AreEqual(30, paramSize, L"Variable parameter size is ok.");
				}
				else
				{
					Assert::AreEqual(MessageBuilder::GetParameterSize(paramType), paramSize, L"Parameter size is ok.");
				}

				switch (i)
				{
				case 0:
					Assert::AreEqual(0x2A, *reinterpret_cast<const int*>(paramData), L"Int32 parameter is ok.");
					break;
				case 1:
					Assert::AreEqual(0.12f, *reinterpret_cast<const float*>(paramData), L"Float32 parameter is ok.");
					break;
				case 2:
					Assert::AreEqual(0, std::strcmp("Something wonderful may happen", reinterpret_cast<const char*>(paramData)), L"String parameter is ok.");
					break;
				}
			}
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
