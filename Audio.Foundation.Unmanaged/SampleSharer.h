#pragma once

#include "ISampleSharer.h"
#include <vector>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			_APIDEF class SampleSharer : public Audio::Foundation::Unmanaged::Abstractions::ISampleSharer
			{
			public:
				SampleSharer();
				virtual ~SampleSharer();


				virtual Abstractions::IChannelLink* get_Send(int iIdx);

				virtual void AddSend(Abstractions::ISampleContainer& fromChannel, Abstractions::ISampleReceiver& channel, float volume, float pan);
				virtual void RemoveSend(Abstractions::ISampleReceiver& channel);
				virtual void RemoveAllSends();

				virtual void RouteToSends();

			private:
				std::vector<Abstractions::IChannelLink*> m_vecSends;
			};
		}
	}
}
