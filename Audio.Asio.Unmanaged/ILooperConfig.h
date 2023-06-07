#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <comdef.h>

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_ASIO_UNMANAGED_API _declspec(uuid("7bc8271a-2bcb-4adb-a8e8-3b8bd71ea07a")) ILooperConfig : public IUnknown
				{
					const wchar_t* get_Name() const = 0;
					void put_Name(const wchar_t* value) = 0;
					_declspec(property(get = get_Name, put = put_Name)) const wchar_t* Name;

					unsigned int get_MidiInput() const = 0;
					void put_MidiInput(unsigned int value) = 0;
					_declspec(property(get = get_MidiInput, put = put_MidiInput)) unsigned int& MidiInput;

					const IID& get_AsioDevice() const = 0;
					void put_AsioDevice(const IID& value) = 0;
					_declspec(property(get = get_AsioDevice, put = put_AsioDevice)) const IID& AsioDevice;

					int get_InputChannel(int index) const = 0;
					void put_InputChannel(int index, int value) = 0;
					_declspec(property(get = get_InputChannel, put = put_InputChannel)) int InputChannel[];

					const int* get_InputChannelList() const = 0;
					_declspec(property(get = get_InputChannelList)) const int* InputChannelList;

					size_t get_InputChannelCount() const = 0;
					_declspec(property(get = get_InputChannelCount)) size_t InputChannelCount;

					int get_OutputChannel(int index) const = 0;
					void put_OutputChannel(int index, int value) = 0;
					_declspec(property(get = get_OutputChannel, put = put_OutputChannel)) int OutputChannel[];

					const int* get_OutputChannelList() const = 0;
					_declspec(property(get = get_OutputChannelList)) const int* OutputChannelList;

					size_t get_OutputChannelCount() const = 0;
					_declspec(property(get = get_OutputChannelCount)) size_t OutputChannelCount;

					int get_SampleCount() const = 0;
					void put_SampleCount(int value) = 0;
					_declspec(property(get = get_SampleCount, put = put_SampleCount)) int& SampleCount;

					float get_OutputSaturation() const = 0;
					void put_OutputSaturation(float value) = 0;
					_declspec(property(get = get_OutputSaturation, put = put_OutputSaturation)) float& OutputSaturation;

					void AddInputChannel(int id) = 0;
					void AddOutputChannel(int id) = 0;

					void AddInputChannelList(int idList[], size_t count) = 0;
					void AddOutputChannelList(int idList[], size_t count) = 0;
				};

				_AUDIO_ASIO_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ILooperConfig, __uuidof(ILooperConfig));
			}
		}
	}
}