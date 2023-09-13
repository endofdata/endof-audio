#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("BB9D115D-5275-4ABE-8AC9-02603EA8D557")) INetClient : public IUnknown
				{
					int Send(const char* buffer, int size) = 0;
					const char* Receive(int& size) = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(INetClient, __uuidof(INetClient));
			}
		}
	}
}