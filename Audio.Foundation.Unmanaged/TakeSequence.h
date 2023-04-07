#pragma once

#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ITakeSequence.h"
#include "ISampleProcessor.h"
#include "Transport.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class TakeSequence : public ITakeSequence, public ISampleProcessor
			{
			public:
				TakeSequence(ITransportPtr& transport);
				virtual ~TakeSequence();

				int get_TakeCount();

				ITakePtr get_Take(int index);

				int AddTake(ITakePtr& item);
				bool RemoveTake(int takeId);
				bool MoveTake(int takeId, AudioTime to);
				ITakePtr FindTake(int takeId);

				AudioTime get_PlayPosition();
				void put_PlayPosition(AudioTime value);

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:

				ITransportPtr m_transport;

				std::vector<ITakePtr> m_takes;
				std::vector<ITakePtr>::iterator m_playPosition;
				AudioTime m_currentTime;

				std::vector<ITakePtr>::iterator m_scheduledTake;
				AudioTime m_scheduledTime;

				bool m_isBypassed;
			};
		}
	}
}