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

				ISampleProcessorPtr get_Next();
				void put_Next(ISampleProcessorPtr value);

				bool get_HasNext();

				void Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleProcessorPtr m_pNext;
				ISampleContainerPtr m_pTarget;
			};
		}
	}
}