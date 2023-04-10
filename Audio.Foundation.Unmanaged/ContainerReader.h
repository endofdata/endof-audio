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
				ContainerReader(ISampleContainerPtr& source);
				virtual ~ContainerReader();

				int ReadSamples(ISampleContainerPtr& container, bool overdub);

				bool get_IsLooping();
				void put_IsLooping(bool value);

				int get_SamplePosition() const;
				void put_SamplePosition(int value);

				int get_ChannelCount() const;
				void put_ChannelCount(int value);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleContainerPtr m_container;
				int m_samplePosition;
				SampleContainerSpan m_span;
				bool m_isLooping;
			};
		}
	}
}

