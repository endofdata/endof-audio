#pragma once

#include "Audio.Osc.Unamanged.h"

#include <streambuf>
#include <istream>

namespace Audio
{
    namespace Osc
    {
        namespace Unmanaged
        {
            class _AUDIO_OSC_UNMANAGED_API MemoryBuffer : public std::basic_streambuf<char>
            {
            public:
                MemoryBuffer(std::istream& istr, size_t size);

                size_t get_Size() const;
                _declspec(property(get = get_Size)) size_t Size;

            private:
                std::unique_ptr<char[]> m_buffer;
                size_t m_size;
            };
        }
    }
}