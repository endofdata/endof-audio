#pragma once

#include "ISampleContainer.h"
#include "ISampleSource.h"
#include "UnknownBase.h"
#include <istream>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class StreamReader : public ISampleSource
			{
			public:
				StreamReader(const GUID& id, std::istream& input, int channelCount);
				virtual ~StreamReader();

				const GUID& get_Id() const;

				int ReadSamples(ISampleContainerPtr& container, const MixParameter& mix, bool overdub);

				bool get_IsLooping() const;
				void put_IsLooping(bool value);

				int get_SamplePosition() const;
				void put_SamplePosition(int value);

				int get_ChannelCount() const;
				void put_ChannelCount(int value);

				DECLARE_IUNKNOWN

			private:
				GUID m_id;
				int m_channelCount;
				std::istream& m_input;
				std::streampos m_initialPos;
				bool m_isLooping;
			};
		}
	}
}

