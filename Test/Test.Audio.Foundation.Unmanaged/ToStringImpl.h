#pragma once

#include "CppUnitTest.h"
#include <ITake.h>
#include <format>
#include <sstream>
#include <AudioTime.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Microsoft 
{
    namespace VisualStudio 
    {
        namespace CppUnitTestFramework 
        {
            template<> static inline std::wstring ToString(const AudioTime& time)
            {
                return time.ToString();
            }

            template<> static inline std::wstring ToString(const ITakePtr& take)
            {
                std::wostringstream oss;
                oss << L"Take[" << take->Id << L"] [" << take->Position.ToString() << L"] to [" << take->EndPosition.ToString() << L']';
                return oss.str();
            }
        }
    }
}