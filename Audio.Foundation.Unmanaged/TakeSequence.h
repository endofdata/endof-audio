#pragma once

#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ITakeSequence.h"
#include "ISampleProcessor.h"
#include "IHostClock.h"
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
				TakeSequence(IHostClockPtr& hostClock, ISampleContainerPtr& targetContainer);
				virtual ~TakeSequence();

				virtual int get_TakeCount();

				virtual ITakePtr get_Take(int index);

				virtual int AddTake(ITakePtr& item);
				virtual bool RemoveTake(int takeId);
				virtual bool MoveTake(int takeId, AudioTime to);
				virtual ITakePtr FindTake(int takeId);

				virtual AudioTime get_PlayPosition();
				virtual void put_PlayPosition(AudioTime value);

				ISampleProcessorPtr& get_next();
				void put_Next(ISampleProcessorPtr &value);

				bool get_HasNext();

				void Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:

				IHostClockPtr m_pHostClock;

				std::vector<ITakePtr> m_takes;
				std::vector<ITakePtr>::iterator m_playPosition;
				AudioTime m_currentTime;

				std::vector<ITakePtr>::iterator m_scheduledTake;
				AudioTime m_scheduledTime;

				ISampleProcessorPtr m_pNext;
				ISampleContainerPtr m_pTargetContainer;
			};
		}
	}
}