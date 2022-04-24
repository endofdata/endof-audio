#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface IChannelLink;

				__interface ISampleLinker
				{
					_declspec(property(get = get_OutputLink, put = put_OutputLink)) IChannelLink* OutputLink;

					IChannelLink* get_OutputLink() = 0;
					void put_OutputLink(IChannelLink* value) = 0;

					void Send() = 0;
				};
			}
		}
	}
}
