#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <IController.h>
#include <ITransport.h>
#include <comdef.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				typedef IController* (*ControllerFactoryFunc)(ITransportPtr& transport);

				__interface _AUDIO_ASIO_UNMANAGED_API _declspec(uuid("7bc8271a-2bcb-4adb-a8e8-3b8bd71ea07a")) ILooperConfig : public IUnknown
				{
					const wchar_t* get_Name() const = 0;
					void put_Name(const wchar_t* value) = 0;
					_declspec(property(get = get_Name, put = put_Name)) const wchar_t* Name;

					unsigned int get_MidiInput() const = 0;
					void put_MidiInput(unsigned int value) = 0;
					/// <summary>
					/// Gets or sets the device ID of the MIDI device that acts as looper controller.
					/// </summary>
					/// <remarks>
					/// If no MIDI device is used as looper controller, set <c>MidiInput</c> to -1 and define a <see cref="ControllerFactory"/> instead.
					/// </remarks>
					_declspec(property(get = get_MidiInput, put = put_MidiInput)) unsigned int& MidiInput;

					ControllerFactoryFunc get_ControllerFactory() const = 0;
					void put_ControllerFactory(ControllerFactoryFunc value) = 0;
					/// <summary>
					/// Gets or sets the factory method to create a controller, if no MIDI controller is to be used.
					/// </summary>
					/// <remarks>
					/// <para>The factory method receives the <see cref="ITransportPtr"/> for the ASIO device so that the controller
					/// can provide a <see cref="ControllerCode"/> also for transport-related events like 'ControllerCode::Locate'.
					/// </para>
					/// <para>To ues a MIDI device as looper controller, set the <see cref="MidiInput"/> to the corresponding device ID
					/// and the <c>ControllerFactory</c> to <see langword="null"/>.
					/// </para>
					/// </remarks>
					_declspec(property(get = get_ControllerFactory, put = put_ControllerFactory)) ControllerFactoryFunc ControllerFactory;

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

					int get_ControlResolution() const = 0;
					void put_ControlResolution(int value) = 0;
					_declspec(property(get = get_ControlResolution, put = put_ControlResolution)) int ControlResolution;

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