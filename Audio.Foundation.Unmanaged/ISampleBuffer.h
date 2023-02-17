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
				/// An unmanaged memory buffer for samples in internal <see cref="sample"> format
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("7df79f36-35fb-4449-b72d-5952c1f4fdbc")) ISampleBuffer : public IUnknown
				{
					void Clear() = 0;

					/// <summary>
					/// The sample buffer
					/// </summary>
					_declspec(property(get = get_Sample, put = put_Sample)) sample Samples[]; //!< Gets or sets the sample at the given index

					/// <summary>
					/// Gets the sample at the specified <paramref name="index"/>
					/// </summary>
					/// <param name="index">Zero-based sample index</param>
					/// <returns>Sample value</returns>
					sample get_Sample(int index) = 0;

					/// <summary>
					/// Sets the sample <paramref name="value"/> at the specified <paramref name="index"/>
					/// </summary>
					/// <param name="index">Zero-based sample index</param>
					/// <param name="value">Sample value to set</param>
					void put_Sample(int index, sample value) = 0;

					_declspec(property(get = get_SamplePtr)) sample* SamplePtr; //!< Gets the address of the unmanaged sample buffer

					sample* get_SamplePtr() = 0;

					_declspec(property(get = get_SampleCount)) int SampleCount; //!< Gets the maximum number of samples in the buffer

					int get_SampleCount() = 0;
				};

				_COM_SMARTPTR_TYPEDEF(ISampleBuffer, __uuidof(ISampleBuffer));
			}
		}
	}
}
