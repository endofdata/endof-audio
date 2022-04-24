#pragma once

#include <asio.h>

namespace Audio
{
	namespace Asio
	{
		namespace Debug
		{
			// max 32 bytes incl. terminating zero
			const int MaxAsioDriverName = 32;

			// max 124 bytes incl.
			const int MaxAsioErrorMessage = 124;

			class DriverDescription
			{
			public:
				DriverDescription();
				DriverDescription(const DriverDescription& other);

				virtual ~DriverDescription();

				DriverDescription& operator = (const DriverDescription& other);

				char* get_DriverName() const;
				void put_DriverName(char* value);

				long get_DriverVersion() const;
				void put_DriverVersion(long value);

				long get_InputChannels() const;
				void put_InputChannels(long value);

				long get_OutputChannels() const;
				void put_OutputChannels(long value);

				long get_InputLatency() const;
				void put_InputLatency(long value);

				long get_OutputLatency() const;
				void put_OutputLatency(long value);

				long get_MinBufferSize() const;
				void put_MinBufferSize(long value);

				long get_MaxBufferSize() const;
				void put_MaxBufferSize(long value);

				long get_PreferredBufferSize() const;
				void put_PreferredBufferSize(long value);

				long get_Granularity() const;
				void put_Granularity(long value);

				ASIOSampleRate get_SampleRate() const;
				void put_SampleRate(ASIOSampleRate value);

				const ASIOClockSource& get_ClockSource(int index) const;
				void put_ClockSource(int index, const ASIOClockSource& value);

				long get_NumClockSources() const;
				void put_NumClockSources(long value);

				_declspec(property(get = get_DriverName, put = put_DriverName)) char* DriverName;
				_declspec(property(get = get_DriverVersion, put = put_DriverVersion)) long DriverVersion;
				_declspec(property(get = get_InputChannels, put = put_InputChannels)) long InputChannels;
				_declspec(property(get = get_OutputChannels, put = put_OutputChannels)) long OutputChannels;
				_declspec(property(get = get_InputLatency, put = put_InputLatency)) long InputLatency;
				_declspec(property(get = get_OutputLatency, put = put_OutputLatency)) long OutputLatency;
				_declspec(property(get = get_MinBufferSize, put = put_MinBufferSize)) long MinBufferSize;
				_declspec(property(get = get_MaxBufferSize, put = put_MaxBufferSize)) long MaxBufferSize;
				_declspec(property(get = get_PreferredBufferSize, put = put_PreferredBufferSize)) long PreferredBufferSize;
				_declspec(property(get = get_Granularity, put = put_Granularity)) long Granularity;
				_declspec(property(get = get_SampleRate, put = put_SampleRate)) ASIOSampleRate SampleRate;
				_declspec(property(get = get_ClockSource, put = put_ClockSource)) ASIOClockSource ClockSource[]; 
				_declspec(property(get = get_NumClockSources, put = put_NumClockSources)) long NumClockSources;

				const ASIOClockSource* GetClockSourceArray(long& numClockSourcesOut) const;

			private:
				char* m_pDriverName;
				long m_lDriverVersion;
				long m_lInputChannels;
				long m_lOutputChannels;
				long m_lInputLatency;
				long m_lOutputLatency;
				long m_lMinBufferSize;
				long m_lMaxBufferSize;
				long m_lPreferredBufferSize;
				long m_lGranularity;
				ASIOSampleRate m_sampleRate;
				ASIOClockSource* m_pClockSources;
				long m_lNumClockSources;
			};

		}
	}
}
