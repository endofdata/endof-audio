#pragma once


#include "ISampleContainer.h"
#include "ISampleSource.h"
#include "UnknownBase.h"
#include "SampleContainerSpan.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class ContainerReader : public ISampleSource
			{
			public:
				ContainerReader(const GUID& id, ISampleContainerPtr& source);
				virtual ~ContainerReader();

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
				ISampleContainerPtr m_container;
				int m_samplePosition;
				SampleContainerSpan m_span;
				bool m_isLooping;
			};
		}
	}
}

