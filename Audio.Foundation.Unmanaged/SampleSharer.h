#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleSharer.h"
#include <vector>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleSharer : public Audio::Foundation::Unmanaged::Abstractions::ISampleSharer
			{
			public:
				SampleSharer();
				virtual ~SampleSharer();

				virtual void put_Source(Abstractions::ISampleContainer* value);
				virtual Abstractions::ISampleContainer* get_Source();

				virtual Abstractions::ISampleReceiver* get_Target(int iIdx);

				virtual void AddTarget(Abstractions::ISampleReceiver& channel);
				virtual void RemoveTarget(Abstractions::ISampleReceiver& channel);
				virtual void RemoveAllTargets();

				virtual void RouteToTargets();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				Abstractions::ISampleContainer* m_pSource;
				std::vector<Abstractions::ISampleReceiver*> m_vecTargets;
			};
		}
	}
}
