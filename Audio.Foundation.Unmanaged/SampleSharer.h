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
			class SampleSharer :public UnknownBase, public Audio::Foundation::Unmanaged::Abstractions::ISampleSharer
			{
			public:
				SampleSharer();
				virtual ~SampleSharer();

				virtual Abstractions::IChannelLink* get_Send(int iIdx);

				virtual void AddSend(Abstractions::ISampleContainer& fromChannel, Abstractions::ISampleReceiver& channel, float volume, float pan);
				virtual void RemoveSend(Abstractions::ISampleReceiver& channel);
				virtual void RemoveAllSends();

				virtual void RouteToSends();

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				std::vector<Abstractions::IChannelLink*> m_vecSends;
			};
		}
	}
}
