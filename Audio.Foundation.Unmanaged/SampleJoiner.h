#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleReceiver.h"
#include "ISampleJoiner.h"
#include "SampleContainer.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleJoiner : public SampleContainer, public Audio::Foundation::Unmanaged::Abstractions::ISampleReceiver, public Audio::Foundation::Unmanaged::Abstractions::ISampleJoiner
			{
			public:
				SampleJoiner(int sampleCount);
				virtual ~SampleJoiner();

				virtual void Flush();
				virtual void Receive(Abstractions::IChannelLink& inputBuffer);

				virtual Abstractions::IChannelLink* get_OutputLink();
				virtual void put_OutputLink(Abstractions::IChannelLink* value);

				virtual void Send();

				void MixInput(float* pSourceLeft, float* pSourceRight, float volume, float pan);

			protected:
				void MixInput(ISampleContainer* pInput, float volume, float pan);
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				Abstractions::IChannelLink* m_pOutputLink;
			};
		}
	}
}
