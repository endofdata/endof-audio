#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <asio.h>

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			enum class _AUDIO_ASIO_UNMANAGED_API AsioError
			{
				None = ASE_OK,								// This value will be returned whenever the call succeeded

				Success = ASE_SUCCESS,						// unique success return value for ASIOFuture calls

				NotPresent = ASE_NotPresent,				// hardware input or output is not present or available

				HWMalfunction = ASE_HWMalfunction,			// hardware is malfunctioning (can be returned by any ASIO function)

				InvalidParameter = ASE_InvalidParameter,	// input parameter invalid

				InvalidMode = ASE_InvalidMode,				// hardware is in a bad mode or used in a bad mode

				SPNotAdvancing = ASE_SPNotAdvancing,		// hardware is not running when sample position is inquired

				NoClock = ASE_NoClock,						// sample clock or rate cannot be determined or is not present

				NoMemory = ASE_NoMemory						// not enough memory for completing the request
			};
		}
	}
}
