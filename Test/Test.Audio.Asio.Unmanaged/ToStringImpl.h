#pragma once

#include "CppUnitTest.h"
#include <ILooperEvents.h>

using namespace Audio::Asio::Unmanaged::Abstractions;

namespace Microsoft 
{
    namespace VisualStudio 
    {
        namespace CppUnitTestFramework 
        {
            template<> static inline std::wstring ToString(const RecordingStatusType& status)
            {
                switch (status)
                {
                case RecordingStatusType::Off:
                    return L"Off";
                case RecordingStatusType::Armed:
                    return L"Armed";
                case RecordingStatusType::Recording:
                    return L"Recording";
                case RecordingStatusType::Unarmed:
                    return L"Unarmed";
                default:
                    return L"<invalid>";
                }
            }
        }
    }
}