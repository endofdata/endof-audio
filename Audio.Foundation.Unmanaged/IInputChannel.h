#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>


namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface ISampleContainer;
				__interface ISampleSharer;
				__interface ISampleReceiver;
				__interface IOutputChannelPair;


				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("8a41766a-92ae-4f1d-b996-f8d4b98105ad")) IInputChannel : public IUnknown
				{
					ISampleContainer& get_SampleContainer() = 0;

					_declspec(property(get = get_SampleContainer)) ISampleContainer& SampleContainer;

					ISampleSharer& get_SampleSharer() = 0;

					_declspec(property(get = get_SampleSharer)) ISampleSharer& SampleSharer;

					int get_SampleType() = 0;

					_declspec(property(get = get_SampleType)) int SampleType;


					IOutputChannelPair* get_Monitor() = 0;
					void put_Monitor(IOutputChannelPair* value) = 0;

					_declspec(property(get = get_Monitor, put = put_Monitor)) IOutputChannelPair* Monitor;

					void Swap(bool readSecondHalf) = 0;
					void Send() = 0;
				};
			}
		}
	}
}
