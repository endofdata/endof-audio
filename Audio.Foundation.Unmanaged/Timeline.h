#pragma once

#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ITimeline.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class Timeline : public ITimeline
			{
			public:
				Timeline();
				virtual ~Timeline();

				virtual int get_TakeCount();

				virtual ITakePtr get_Take(int id);

				virtual int AddTake(ITakePtr item);
				virtual bool RemoveTake(int id);
				virtual bool MoveTake(int id, Time to);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ITakePtr GetTakeById(int id);

				std::vector<ITakePtr> m_takes;
			};
		}
	}
}