#pragma once
#include <ILooperEvents.h>
#include <UnknownBase.h>

using namespace Audio::Asio::Unmanaged::Abstractions;

namespace RepeatMyself
{
	class CommandLine
	{
	public:
		static const int MAX_CHANNELS = 16;

		CommandLine();
		virtual ~CommandLine();

		bool get_IsSessionRecording() const;
		_declspec(property(get = get_IsSessionRecording)) bool IsSessionRecording;

		int get_Input(int index) const;
		_declspec(property(get = get_Input)) int Input[];

		int get_InputCount() const;
		_declspec(property(get = get_InputCount)) int InputCount;

		const int* get_InputList() const;
		_declspec(property(get = get_InputList)) int* InputList;

		int get_Output(int index) const;
		_declspec(property(get = get_Output)) int Output[];

		int get_OutputCount() const;
		_declspec(property(get = get_OutputCount)) int OutputCount;

		const int* get_OutputList() const;
		_declspec(property(get = get_OutputList)) int* OutputList;

		const char* get_MidiDevice() const;
		_declspec(property(get = get_MidiDevice)) const char* MidiDevice;

		float get_InputSaturation() const;
		_declspec(property(get = get_InputSaturation)) float InputSaturation;

		float get_OutputSaturation() const;
		_declspec(property(get = get_OutputSaturation)) float OutputSaturation;

		int get_Priority() const;
		_declspec(property(get = get_Priority)) int Priority;

		static CommandLine FromArgs(int argc, char* argv[]);

	private:
		static int SplitIntList(const char* text, int values[], int maxValues);

		int m_isSessionRecording;
		int m_input[MAX_CHANNELS];
		int m_inputCount;
		int m_output[MAX_CHANNELS];
		int m_outputCount;
		int m_priority;

		std::string m_midiDevice;
		float m_inputSaturation;
		float m_outputSaturation;
	};
}
