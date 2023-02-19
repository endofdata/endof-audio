#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleJoiner.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("5d96f162-5569-40b6-9cc1-529800d1eaa2")) IOutputChannelPair : public IUnknown
				{
					int get_SampleType() = 0;

					/// <summary>
					/// Gets or sets the sample type provided by this input
					/// </summary>
					_declspec(property(get = get_SampleType)) int SampleType;


					bool get_IsActive() = 0;
					void put_IsActive(bool value) = 0;

					/// <summary>
					/// Gets or sets a value indicating whether this channel is active
					/// </summary>
					_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;


					/// <summary>
					/// Processes the next audio buffer
					/// </summary>
					/// <param name="readSecondHalf">ASIO specific: is this the second half-buffer?</param>
					void OnNextBuffer(bool writeSecondHalf) = 0;

					/// <summary>
					/// Processed native sample buffers for direct monitoring
					/// </summary>
					/// <param name="pBuffer">Native sample buffer</param>
					/// <param name="fLeft">Level for left channel</param>
					/// <param name="fRight">Level for right channel</param>
					void DirectOut(void* pBuffer, bool fLeft, bool fRight) = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IOutputChannelPair, __uuidof(IOutputChannelPair));
			}
		}
	}
}
