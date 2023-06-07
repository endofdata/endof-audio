#pragma once
#include <UnknownBase.h>
#include <comdef.h>
#include <ILooperConfig.h>

#define LOOPER_MAX_INPUT_CHANNELS 16
#define LOOPER_MAX_OUTPUT_CHANNELS 16

using namespace Audio::Asio::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			class LooperConfig : public ILooperConfig
			{
			public:
				LooperConfig();

				const wchar_t* get_Name() const;
				void put_Name(const wchar_t* value);

				unsigned int get_MidiInput() const;
				void put_MidiInput(unsigned int value);

				const IID& get_AsioDevice() const;
				void put_AsioDevice(const IID& value);

				int get_InputChannel(int index) const;
				void put_InputChannel(int index, int value);

				const int* get_InputChannelList() const;

				size_t get_InputChannelCount() const;

				int get_OutputChannel(int index) const;
				void put_OutputChannel(int index, int value);

				const int* get_OutputChannelList() const;

				size_t get_OutputChannelCount() const;

				int get_SampleCount() const;
				void put_SampleCount(int value);

				float get_OutputSaturation() const;
				void put_OutputSaturation(float value);

				void AddInputChannel(int id);
				void AddOutputChannel(int id);

				void AddInputChannelList(int idList[], size_t count);
				void AddOutputChannelList(int idList[], size_t count);

				DECLARE_IUNKNOWN

			private:
				std::wstring m_name;
				unsigned int m_midiInput;
				IID m_asioDevice;
				int m_inputChannelIds[LOOPER_MAX_INPUT_CHANNELS];
				size_t m_numInputIds;
				int m_outputChannelIds[LOOPER_MAX_OUTPUT_CHANNELS];
				size_t m_numOutputIds;
				int m_sampleCount;
				float m_outputSaturation;
			};

		}
	}
}
