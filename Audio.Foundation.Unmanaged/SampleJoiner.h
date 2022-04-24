#pragma once

#include "ISampleReceiver.h"
#include "ISampleLinker.h"
#include "SampleContainer.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			_APIDEF class SampleJoiner : public SampleContainer, public Audio::Foundation::Unmanaged::Abstractions::ISampleReceiver, public Audio::Foundation::Unmanaged::Abstractions::ISampleLinker
			{
			public:
				SampleJoiner(int sampleCount);

				virtual void Flush();
				virtual void Receive(Abstractions::IChannelLink& inputBuffer);

				virtual Abstractions::IChannelLink* get_OutputLink();
				virtual void put_OutputLink(Abstractions::IChannelLink* value);

				virtual void Send();

				void MixInput(float* pSourceLeft, float* pSourceRight, float volume, float pan);

			protected:
				void MixInput(ISampleContainer* pInput, float volume, float pan);

			private:
				Abstractions::IChannelLink* m_pOutputLink;
			};
		}
	}
}
