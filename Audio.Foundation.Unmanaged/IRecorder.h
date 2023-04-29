#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <ISampleContainer.h>
#include <comdef.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("74926894-9de0-4a0b-bd6b-c90a7fe4ba1a")) IRecorder : public IUnknown
				{
					/// <summary>
					/// Creates an audio take from current recording data.
					/// </summary>
					/// <param name="continueRecording">Controls whether recorder should go on with next recording</param>
					ISampleContainerPtr CreateSampleContainer(bool continueRecording, int fadeIn, int fadeOut) = 0;

					/// <summary>
					/// Drops any recording data.
					/// </summary>
					/// <param name="continueRecording">Controls whether recorder should go on with next recording</param>
					void DropRecording(bool continueRecording) = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IRecorder, __uuidof(IRecorder));
			}
		}
	}
}
