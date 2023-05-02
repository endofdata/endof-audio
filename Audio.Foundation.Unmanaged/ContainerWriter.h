#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
#include "ISampleContainer.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class ContainerWriter : public ISampleProcessor
			{
			public:
				ContainerWriter(ISampleContainerPtr& target);
				virtual ~ContainerWriter();

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				DECLARE_IUNKNOWN

			private:
				ISampleContainerPtr m_pTarget;
				bool m_isBypassed;
			};
		}
	}
}