#pragma once

#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ITimeline.h"
#include "ISampleProcessor.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class Timeline : public ITimeline, public ISampleProcessor
			{
			public:
				Timeline();
				virtual ~Timeline();

				virtual int get_TakeCount();

				virtual ITakePtr get_Take(int index);

				virtual int AddTake(ITakePtr item);
				virtual bool RemoveTake(int takeId);
				virtual bool MoveTake(int takeId, Time to);
				virtual ITakePtr FindTake(int takeId);

				ISampleProcessorPtr get_Next();
				void put_Next(ISampleProcessorPtr value);

				bool get_HasNext();

				void Process(ISampleContainerPtr container);
				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:

				bool m_isActive;

				std::vector<ITakePtr> m_takes;
				std::vector<ITakePtr>::iterator m_playPosition;

				ISampleProcessorPtr m_pNext;
			};
		}
	}
}