#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <comdef.h>
#include <ISampleContainer.h>
#include <ISampleReceiver.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				/// <summary>
				/// Exposes methods and properties that 
				/// </summary>
				// TODO: Do we nned this? Most interface clients use either the Input or the Output only. But SampleSharer uses it internally
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("866238bb-6fb5-4f00-ae6d-5316f4df7cec")) IChannelLink : public IUnknown
				{
					float get_Level() = 0;
					void put_Level(float value) = 0;

					/// <summary>
					/// Gets or sets the level applied to the input signal
					/// </summary>
					_declspec(property(get = get_Level, put = put_Level)) float Level;

					float get_Pan() = 0;
					void put_Pan(float value) = 0;

					/// <summary>
					/// Gets or sets the panorama position applied to the input signal
					/// </summary>
					_declspec(property(get = get_Pan, put = put_Pan)) float Pan;

					ISampleContainer* get_Input() = 0;
					void put_Input(ISampleContainer* input) = 0;

					/// <summary>
					/// Gets or sets the <see cref="ISampleContainer"/> providing the input signal
					/// </summary>
					_declspec(property(get = get_Input, put = put_Input)) ISampleContainer* Input;

					ISampleReceiver* get_Output() = 0;
					void put_Output(ISampleReceiver* Output) = 0;

					/// <summary>
					/// Gets or sets the <see cref="ISampleReceiver"/> for the output signal
					/// </summary>
					_declspec(property(get = get_Output, put = put_Output)) ISampleReceiver* Output;

					bool get_HasOutput() = 0;

					/// <summary>
					/// Gets a value indicating whether an <see cref="Output"/> is specified
					/// </summary>
					_declspec(property(get = get_HasOutput)) bool HasOutput;

					bool get_HasInput() = 0;

					/// <summary>
					/// Gets a value indicating whether an <see cref="Input"/> is specified
					/// </summary>
					_declspec(property(get = get_HasInput)) bool HasInput;
				};
			}
		}
	}
}
