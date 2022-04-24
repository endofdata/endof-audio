#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "IChannelLink.h"
#include "UnknownBase.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class ChannelLink : public Audio::Foundation::Unmanaged::Abstractions::IChannelLink
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

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				Abstractions::ISampleContainer* m_pInput;
				Abstractions::ISampleReceiver* m_pOutput;
				float m_volume;
				float m_pan;
			};
		}
	}
}
