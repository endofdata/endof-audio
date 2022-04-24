#pragma once

#include "IChannelLink.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			_APIDEF class ChannelLink : public Audio::Foundation::Unmanaged::Abstractions::IChannelLink
			{
			public:
				ChannelLink();
				ChannelLink(Abstractions::ISampleContainer* pInput, Abstractions::ISampleReceiver* pOutput, float volume, float pan);
				virtual ~ChannelLink();

				float get_Volume();
				void put_Volume(float value);

				float get_Pan();
				void put_Pan(float value);

				Abstractions::ISampleContainer* get_Input();
				void put_Input(Abstractions::ISampleContainer* input);

				Abstractions::ISampleReceiver* get_Output();
				void put_Output(Abstractions::ISampleReceiver* Output);

			private:
				Abstractions::ISampleContainer* m_pInput;
				Abstractions::ISampleReceiver* m_pOutput;
				float m_volume;
				float m_pan;
			};
		}
	}
}
