#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "IChannelLink.h"
#include "UnknownBase.h"
#include "ISampleContainer.h"
#include "ISampleReceiver.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			/// <summary>
			/// Implementation of <see cref="IChannelLink"/> to combine an input <see cref="ISampleContainer"/> with an output <see cref="ISampleReciver"/>
			/// </summary>
			class ChannelLink : public IChannelLink
			{
			public:
				/// <summary>
				/// Constructor
				/// </summary>
				ChannelLink();

				/// <summary>
				/// Constructor
				/// </summary>
				/// <param name="pInput">Provides the input signal</param>
				/// <param name="pOutput">Receives the processed input signal</param>
				/// <param name="level">Level applied to the input signal</param>
				/// <param name="pan">Panorama position applied to the input signal</param>
				ChannelLink(ISampleContainer* pInput, ISampleReceiver* pOutput, float level, float pan);

				/// <summary>
				/// Destructor
				/// </summary>
				virtual ~ChannelLink();

				virtual float get_Level();
				virtual void put_Level(float value);

				virtual float get_Pan();
				virtual void put_Pan(float value);

				virtual ISampleContainer* get_Input();
				virtual void put_Input(ISampleContainer* input);

				virtual ISampleReceiver* get_Output();
				virtual void put_Output(ISampleReceiver* Output);

				virtual bool get_HasInput();
				virtual bool get_HasOutput();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleContainer* m_pInput;
				ISampleReceiver* m_pOutput;
				float m_level;
				float m_pan;
			};
		}
	}
}
