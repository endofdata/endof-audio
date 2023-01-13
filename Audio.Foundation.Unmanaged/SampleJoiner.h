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
				public Abstractions::ISampleJoiner,
				public Abstractions::ISampleReceiver
			{
			public:
				SampleJoiner(int sampleCount, int channelCount);
				virtual ~SampleJoiner();

				virtual ISampleReceiverPtr get_Target();
				virtual void put_Target(ISampleReceiverPtr value);

				virtual void Receive(Abstractions::ISampleContainerPtr container);

				virtual void Flush();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleReceiverPtr m_pTarget;
			};
		}
	}
}
