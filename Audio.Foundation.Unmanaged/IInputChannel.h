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
				__interface ISampleContainer;
				__interface ISampleSharer;
				__interface ISampleReceiver;
				__interface IOutputChannelPair;


				__interface IInputChannel
				{
					_declspec(property(get = get_AsSampleContainer)) ISampleContainer& AsSampleContainer;

					ISampleContainer& get_AsSampleContainer();

					_declspec(property(get = get_AsSampleSharer)) ISampleSharer& AsSampleSharer;

					ISampleSharer& get_AsSampleSharer();

					_declspec(property(get = get_SampleType)) int SampleType;

					int get_SampleType() = 0;

					_declspec(property(get = get_Monitor, put = put_Monitor)) IOutputChannelPair* Monitor;

					IOutputChannelPair* get_Monitor() = 0;
					void put_Monitor(IOutputChannelPair* value) = 0;

					void Swap(bool readSecondHalf) = 0;
					void Send() = 0;
				};
			}
		}
	}
}
