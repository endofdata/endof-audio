#pragma once
#include <comdef.h>

#define LOOPER_MAX_INPUT_CHANNELS 16
#define LOOPER_MAX_OUTPUT_CHANNELS 16

class LooperConfig
{
public:
	LooperConfig();

	unsigned int get_MidiInput() const;
	void put_MidiInput(unsigned int value);
	_declspec(property(get = get_MidiInput, put = put_MidiInput)) unsigned int& MidiInput;

	const IID& get_AsioDevice() const;
	void put_AsioDevice(const IID& value);
	_declspec(property(get = get_AsioDevice, put = put_AsioDevice)) const IID& AsioDevice;

	int get_InputChannel(int index) const;
	void put_InputChannel(int index, int value);
	_declspec(property(get = get_InputChannel, put = put_InputChannel)) int InputChannel[];

	const int* get_InputChannelList() const;
	_declspec(property(get = get_InputChannelList)) const int* InputChannelList;

	size_t get_InputChannelCount() const;
	_declspec(property(get = get_InputChannelCount)) size_t InputChannelCount;

	int get_OutputChannel(int index) const;
	void put_OutputChannel(int index, int value);
	_declspec(property(get = get_OutputChannel, put = put_OutputChannel)) int OutputChannel[];

	const int* get_OutputChannelList() const;
	_declspec(property(get = get_OutputChannelList)) const int* OutputChannelList;

	size_t get_OutputChannelCount() const;
	_declspec(property(get = get_OutputChannelCount)) size_t OutputChannelCount;

	int get_SampleCount() const;
	void put_SampleCount(int value);
	_declspec(property(get = get_SampleCount, put = put_SampleCount)) int& SampleCount;

	float get_OutputSaturation() const;
	void put_OutputSaturation(float value);
	_declspec(property(get = get_OutputSaturation, put = put_OutputSaturation)) float& OutputSaturation;

	void AddInputChannel(int id);
	void AddOutputChannel(int id);

	void AddInputChannelList(int idList[], size_t count);
	void AddOutputChannelList(int idList[], size_t count);

private:
	unsigned int m_midiInput;
	IID m_asioDevice;
	int m_inputChannelIds[LOOPER_MAX_INPUT_CHANNELS];
	size_t m_numInputIds;
	int m_outputChannelIds[LOOPER_MAX_OUTPUT_CHANNELS];
	size_t m_numOutputIds;
	int m_sampleCount;
	float m_outputSaturation;
};
