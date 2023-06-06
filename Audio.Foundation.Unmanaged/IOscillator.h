#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <comip.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{

				/// <summary>
				/// A signal generator for testing purposes
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("0a3945d6-ac58-4382-8c08-c42dee0f3593")) IOscillator : public IUnknown
				{
					double get_Amplitude() const = 0;
					void put_Amplitude(double value) = 0;
					_declspec(property(get = get_Amplitude, put = put_Amplitude)) double Amplitude;

					double get_Frequency() const = 0;
					void put_Frequency(double value) = 0;
					_declspec(property(get = get_Frequency, put = put_Frequency)) double Frequency;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IOscillator, __uuidof(IOscillator));
			}
		}
	}
}
