#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>
#include <ITake.h>
#include <UnknownBase.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class Take : public ITake
			{
			public:
				Take(int id, ISampleContainerPtr container, Time position, Time length);
				virtual ~Take();

				virtual int get_Id();

				virtual Time get_Position();
				virtual void put_Position(Time value);

				virtual Time get_Length();
				virtual void put_Length(Time value);

				virtual ISampleContainerPtr get_Container();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				int m_id;
				ISampleContainerPtr m_container;
				Time m_position;
				Time m_length;
			};
			
		}
	}
}
