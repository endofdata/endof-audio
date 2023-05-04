#include "pch.h"
#include "MemCheck.h"
#include <memory>
#include <CppUnitTestLogger.h>
#include <CppUnitTestAssert.h>
#include <MemCheck.h>

using namespace Test::Audio::Common;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


using namespace Test::Audio::Common;

MemCheck::MemCheck()
{
	std::memset(&m_preTest, 0, sizeof(m_preTest));
	std::memset(&m_postTest, 0, sizeof(m_postTest));
}

MemCheck::~MemCheck()
{
}

void MemCheck::BeginCheck()
{
	_CrtMemCheckpoint(&m_preTest);
}

void MemCheck::EndCheck()
{
	_CrtMemCheckpoint(&m_postTest);

	_CrtMemState difference;
	std::memset(&difference, 0, sizeof(difference));
	if (_CrtMemDifference(&difference, &m_preTest, &m_postTest))
	{
		_CrtMemDumpStatistics(&difference);
		Assert::Fail(L"Detected memory leaks.");
	}
	else
	{
		Logger::WriteMessage(L"No memory leaks.");
	}
}
