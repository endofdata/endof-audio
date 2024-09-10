#include "pch.h"
#include "CommandLine.h"

using namespace RepeatMyself;

CommandLine::CommandLine() :
	m_isSessionRecording(false),
	m_midiDevice("2- Steinberg UR-RT2-1"),
	m_outputSaturation(1.0)
{
	memset(m_input, 0, sizeof(int) * MAX_CHANNELS);
	memset(m_output, 0, sizeof(int) * MAX_CHANNELS);

	m_input[0] = 1;
	m_inputCount = 1;

	m_output[1] = 1;
	m_outputCount = 2;
}

CommandLine::~CommandLine()
{
}


bool CommandLine::get_IsSessionRecording() const
{
	return m_isSessionRecording;
}

int CommandLine::get_Input(int index) const
{
	if (index < 0 || index >= InputCount)
	{
		throw std::invalid_argument("index must be greater or equal to zero and less than InputCount.");
	}
	return m_input[index];
}

int CommandLine::get_InputCount() const
{
	return m_inputCount;
}

const int* CommandLine::get_InputList() const
{
	return m_input;
}

int CommandLine::get_Output(int index) const
{
	if (index < 0 || index >= OutputCount)
	{
		throw std::invalid_argument("index must be greater or equal to zero and less than OutputCount.");
	}
	return m_output[index];
}

int CommandLine::get_OutputCount() const
{
	return m_outputCount;
}

const int* CommandLine::get_OutputList() const
{
	return m_output;
}

const char* CommandLine::get_MidiDevice() const
{
	return m_midiDevice.c_str();
}

float CommandLine::get_OutputSaturation() const
{
	return m_outputSaturation;
}

CommandLine CommandLine::FromArgs(int argc, char* argv[])
{
	CommandLine commandLine;
	int i = 1;

	while (i < argc)
	{
		char* arg = argv[i++];

		if (!_stricmp(arg, "/session"))
		{
			commandLine.m_isSessionRecording = true;
		}
		else if (!_stricmp(arg, "/in"))
		{
			if (i >= argc)
			{
				throw std::invalid_argument("missing comma-separated list of values after option '/in'.");
			}

			const char* list = argv[i++];
			commandLine.m_inputCount = SplitIntList(list, commandLine.m_input, CommandLine::MAX_CHANNELS);
		}
		else if (!_stricmp(arg, "/out"))
		{
			if (i >= argc)
			{
				throw std::invalid_argument("missing comma-separated list of values after option '/out'.");
			}
			
			const char* list = argv[i++];
			commandLine.m_outputCount = SplitIntList(list, commandLine.m_output, CommandLine::MAX_CHANNELS);
		}
		else if (!_stricmp(arg, "/midi"))
		{
			if (i >= argc)
			{
				throw std::invalid_argument("missing device name after option '/midi'.");
			}

			commandLine.m_midiDevice = argv[i++];
		}
		else if (!_stricmp(arg, "/outsat"))
		{
			if (i >= argc)
			{
				throw std::invalid_argument("missing value after option '/outsat'.");
			}

			commandLine.m_outputSaturation = static_cast<float>(atof(argv[i++]));
		}
	}
	return commandLine;
}


int CommandLine::SplitIntList(const char* text, int values[], int maxValues)
{
	static const char* digits = "0123456789";

	int valueCount = 0;

	while (*text != '\0' && valueCount < maxValues)
	{
		int value = 0;
		bool hasValue = false;

		while (*text != '\0')
		{
			char t = *text++;

			const char* pos = strchr(digits, t);

			if (pos != NULL)
			{
				value *= 10;
				value += pos - digits;
				hasValue = true;
			}
			else if (t == ',')
			{
				break;
			}
			else
			{
				throw std::invalid_argument("invalid character in channel list.");
			}
		}
		if (hasValue)
		{
			values[valueCount++] = value;
		}
	}

	return valueCount;
}