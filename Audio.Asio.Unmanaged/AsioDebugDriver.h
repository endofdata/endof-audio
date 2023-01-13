#pragma once

#include <Audio.Asio.Unmanaged.h>
#include "DriverDescription.h"

// This to have a safe include for IASIO
#include "AsioCore.h"

namespace Audio
{
	namespace Asio
	{
		namespace Debug
		{
			class _AUDIO_ASIO_UNMANAGED_API AsioDebugDriver : public IASIO
			{
			public:
				AsioDebugDriver();
				AsioDebugDriver(const DriverDescription& driverDescription);
				virtual ~AsioDebugDriver();

				virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
				virtual ULONG _stdcall AddRef(void);
				virtual ULONG _stdcall Release(void);

				virtual ASIOBool init(void *sysHandle);
				virtual void getDriverName(char *name);
				virtual long getDriverVersion();
				virtual void getErrorMessage(char *string);
				virtual ASIOError start();
				virtual ASIOError stop();
				virtual ASIOError getChannels(long *numInputChannels, long *numOutputChannels);
				virtual ASIOError getLatencies(long *inputLatency, long *outputLatency);
				virtual ASIOError getBufferSize(long *minSize, long *maxSize,
					long *preferredSize, long *granularity);
				virtual ASIOError canSampleRate(ASIOSampleRate sampleRate);
				virtual ASIOError getSampleRate(ASIOSampleRate *sampleRate);
				virtual ASIOError setSampleRate(ASIOSampleRate sampleRate);
				virtual ASIOError getClockSources(ASIOClockSource *clocks, long *numSources);
				virtual ASIOError setClockSource(long reference);
				virtual ASIOError getSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp);
				virtual ASIOError getChannelInfo(ASIOChannelInfo *info);
				virtual ASIOError createBuffers(ASIOBufferInfo *bufferInfos, long numChannels,
					long bufferSize, ASIOCallbacks *callbacks);
				virtual ASIOError disposeBuffers();
				virtual ASIOError controlPanel();
				virtual ASIOError future(long selector, void *opt);
				virtual ASIOError outputReady();

				DWORD get_InterruptInterval() const;
				void set_InterruptInterval(DWORD value);

				__declspec(property(get = get_InterruptInterval, put = set_InterruptInterval)) DWORD InterruptInterval;

			private:
				DWORD RunInterruptSimulator();
				static DWORD _stdcall InterruptSimulatorThreadEntry(LPVOID lpvThis);

				DriverDescription m_description;
				long m_selectedClockSource;

				void* m_pSysHandle;
				char m_acErrorMessage[Audio::Asio::Unmanaged::MaxAsioErrorMessage + 1];
				bool m_isStarted;
				ASIOSamples m_samplePos;
				ASIOTimeStamp m_timeStamp;

				ASIOCallbacks* m_pCallbacks;
				long m_lNumChannels;
				long m_lBufferSize;
				int* m_pTheBigBufferForAll;

				HANDLE m_hevStop;
				HANDLE m_hThread;
				DWORD m_dwThreadId;
				DWORD m_dwInterruptInterval;

				ULONG m_ulRef;
			};
		}
	}
}
