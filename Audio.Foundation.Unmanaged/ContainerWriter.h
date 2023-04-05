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

				void Process(ISampleContainerPtr& container);

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleContainerPtr m_pTarget;
				bool m_isBypassed;
			};
		}
	}
}