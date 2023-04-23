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
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("7df79f36-35fb-4449-b72d-5952c1f4fdbc")) ISampleBuffer;

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISampleBuffer, __uuidof(ISampleBuffer));

				/// <summary>
				/// An unmanaged memory buffer for samples in internal <see cref="sample"> format
				/// </summary>
				__interface ISampleBuffer : public IUnknown
				{
					void Clear() = 0;

					/// <summary>
					/// The sample buffer
					/// </summary>
					_declspec(property(get = get_Sample, put = put_Sample)) Sample Samples[]; //!< Gets or sets the sample at the given index

					/// <summary>
					/// Gets the sample at the specified <paramref name="index"/>
					/// </summary>
					/// <param name="index">Zero-based sample index</param>
					/// <returns>Sample value</returns>
					Sample get_Sample(int index) const = 0;

					/// <summary>
					/// Sets the sample <paramref name="value"/> at the specified <paramref name="index"/>
					/// </summary>
					/// <param name="index">Zero-based sample index</param>
					/// <param name="value">Sample value to set</param>
					void put_Sample(int index, Sample value) = 0;

					_declspec(property(get = get_SamplePtr)) Sample* SamplePtr; //!< Gets the address of the unmanaged sample buffer

					Sample* get_SamplePtr() = 0;

					_declspec(property(get = get_SampleCount)) int SampleCount; //!< Gets the maximum number of samples in the buffer

					int get_SampleCount() const = 0;

					int WriteTo(ISampleBufferPtr& other, int sampleOffset, int sampleCount, int targetOffset, double level, bool overdub) const = 0;
				};
			}
		}
	}
}
