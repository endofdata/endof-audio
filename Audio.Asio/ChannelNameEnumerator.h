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
			ref class ChannelNameEnumerator : public System::Collections::Generic::IEnumerator<System::String^>
			{
			public:
				ChannelNameEnumerator(IASIO* pDriver, bool forInput, int numChannels);
				virtual ~ChannelNameEnumerator();
				!ChannelNameEnumerator();

				property System::String^ Current
				{
					virtual System::String^ get() = System::Collections::Generic::IEnumerator<System::String^>::Current::get;
				}

				virtual bool MoveNext() = System::Collections::Generic::IEnumerator<System::String^>::MoveNext;

				virtual void Reset() = System::Collections::Generic::IEnumerator<System::String^>::Reset;

				property System::Object^ NonGenericCurrent
				{
					virtual System::Object^ get() = System::Collections::IEnumerator::Current::get;
				}

			private:
				void ThrowIfFailed(ASIOError error);
				void CleanUp();

				IASIO* m_pDriver;
				bool m_isInput;
				int m_currentId;
				int m_maxId;
				System::String^ m_currentName;
			};
		}
	}
}