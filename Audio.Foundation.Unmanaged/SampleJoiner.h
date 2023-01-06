#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleReceiver.h"
#include "ISampleJoiner.h"
#include "ISampleContainer.h"
#include "SampleContainerBase.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleJoiner : public SampleContainerBase, 
				public Audio::Foundation::Unmanaged::Abstractions::ISampleReceiver, 
				public Audio::Foundation::Unmanaged::Abstractions::ISampleJoiner,
				public Audio::Foundation::Unmanaged::Abstractions::ISampleContainer
			{
			public:
				SampleJoiner(int sampleCount);
				virtual ~SampleJoiner();

				virtual void Flush();
				virtual void Receive(IChannelLink& inputBuffer);

				virtual IChannelLink* get_OutputLink();
				virtual void put_OutputLink(IChannelLink* value);

				virtual void Send();

				virtual void MixInput(float* pSourceLeft, float* pSourceRight, float level, float pan);

				DECLARE_IUNKNOWN

				DECLARE_SAMPLECONTAINER

			protected:
				void MixInput(ISampleContainer* pInput, float level, float pan);
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				IChannelLink* m_pOutputLink;
			};
		}
	}
}
