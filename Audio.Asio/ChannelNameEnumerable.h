#pragma once

#ifndef __ASIODRIVER_FWD_DEFINED__
#include <iasiodrv.h>
#endif

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			ref class ChannelNameEnumerable : public System::Collections::Generic::IEnumerable<System::String^>
			{
			public:
				ChannelNameEnumerable(IASIO* pDriver, bool forInput, int numChannels);
				virtual ~ChannelNameEnumerable();
				!ChannelNameEnumerable();

				virtual System::Collections::Generic::IEnumerator<System::String^>^ GetEnumerator() = System::Collections::Generic::IEnumerable<System::String^>::GetEnumerator; 

				virtual System::Collections::IEnumerator^ GetNonGenericEnumerator() = System::Collections::IEnumerable::GetEnumerator;

			private:
				void CleanUp();

				IASIO* m_pDriver;
				bool m_isInput;
				int m_numChannels;
			};
		}
	}
}