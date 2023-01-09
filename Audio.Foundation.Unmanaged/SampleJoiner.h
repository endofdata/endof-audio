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
				public Abstractions::ISampleReceiver,
				public Abstractions::ISampleContainer
			{
			public:
				SampleJoiner(int sampleCount, int channelCount);
				virtual ~SampleJoiner();

				virtual ISampleReceiver* get_Target();
				virtual void put_Target(ISampleReceiver* value);

				virtual void Receive(Abstractions::ISampleContainer& container);

				virtual void Flush();

				DECLARE_IUNKNOWN

				DECLARE_SAMPLECONTAINER

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleReceiver* m_pTarget;
			};
		}
	}
}
