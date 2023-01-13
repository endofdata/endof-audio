#pragma once

#include <Audio.Asio.Unmanaged.h>

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#ifndef interface
#define interface __interface
#endif
#include <asio.h>
#include <iasiodrv.h>
#include <IInputChannel.h>
#include <IOutputChannelPair.h>
#include "AsioCoreCallbacks.h"

//using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			typedef void (*BufferSwitchEventHandler)(bool writeSecondHalf);

			class _AUDIO_ASIO_UNMANAGED_API AsioCore
			{
			public:
				static const int UseMaximumSize = -1;
				static const int UsePreferredSize = -2;

				static AsioCore* CreateInstance(REFCLSID clsid);


				virtual ~AsioCore();

				void Start();

				void Stop();

				void ShowControlPanel();

				void SetInputMonitoring(int iInputChannel, int iOutputPair);

				void GetDriverName(char* pcOut);

				int GetDriverVersion();

				int GetKnownInputChannel(int index, char* pcOut, int max);

				int GetKnownOutputChannel(int index, char* pcOut, int max);

				_declspec(property(get = get_InputChannelCount)) int InputChannelCount;

				int get_InputChannelCount();

				_declspec(property(get = get_InputChannel)) IInputChannel* InputChannel[];

				IInputChannel* get_InputChannel(int iChannel);

				_declspec(property(get = get_OutputChannelPairCount)) int OutputChannelPairCount;

				int get_OutputChannelPairCount();

				_declspec(property(get = get_OutputChannelPair)) IOutputChannelPair* OutputChannelPair[];

				IOutputChannelPair* get_OutputChannelPair(int iChannel);

				_declspec(property(get = get_SampleCount)) int SampleCount;

				int get_SampleCount();

				_declspec(property(get = get_SampleRate, put = put_SampleRate)) double SampleRate;

				double get_SampleRate();
				void put_SampleRate(double);

				_declspec(property(get = get_SampleType)) ASIOSampleType SampleType;

				ASIOSampleType get_SampleType();

				_declspec(property(get = get_SampleSize)) int SampleSize;

				int get_SampleSize();

				_declspec(property(get = get_BufferSwitchEventHandler, put = put_BufferSwitchEventHandler)) BufferSwitchEventHandler BufferSwitch;

				ASIOError get_LastError();

				_declspec(property(get = get_LastError)) ASIOError LastError;

				BufferSwitchEventHandler get_BufferSwitchEventHandler();
				void put_BufferSwitchEventHandler(BufferSwitchEventHandler value);

				void CreateBuffers(const int inputChannelIds[], int numInputIds, const int outputChannelIds[], int numOutputIds, int sampleCount);

			private:
				struct BufferSize
				{
					int Minimum;
					int Maximum;
					int Preferred;
					int Granularity;

					BufferSize()
					{
						Init();
					}

					void Init()
					{
						Minimum = 0;
						Maximum = 0;
						Preferred = 0;
						Granularity = 0;
					}
				};

				AsioCore();

				void Initialize(REFCLSID clsid);

				void CleanUp();

				void SelectSampleRate();

				void CreateInputChannels(int offset, int count);

				void CreateOutputChannels(int offset, int count);

				void DisposeBuffers();

				void DisposeInputChannels();

				void DisposeOutputChannels();

				void OnBufferSwitch(long doubleBufferIndex, ASIOBool directProcess) const;

				ASIOTime* OnBufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);

				void OnSampleRateDidChange(ASIOSampleRate rate);

				long OnAsioMessage(long selector, long value, void* message, double* opt);

				void ThrowIfFailed(ASIOError error);

				int GetKnownChannel(ASIOBool isInput, int index, char* pcOut, int max);

				IASIO* m_pDriver;

				int m_iInputLatency;
				int m_iOutputLatency;
				int m_iSampleCount;
				BufferSize m_supportedBufferSize;
				bool m_outputReadySupport;

				int m_iHwInputCount;
				int m_iHwOutputCount;
				int m_iHwPinCount;
				ASIOBufferInfo* m_pHwBufferInfo;

				int m_iInputChannels;
				IInputChannel** m_pInputChannels;

				int m_iOutputChannelPairs;
				IOutputChannelPair** m_pOutputChannelPairs;

				int m_iCurrentMonitorInput;

				ASIOSampleRate m_sampleRate;

				ASIOSampleType m_sampleType;

				BufferSwitchEventHandler m_bufferSwitchEventHandler;

				ASIOError m_lastError;
				AsioCoreCallbacks* m_pCoreCallbacks;

				friend class AsioCoreCallbacks;
			};
		}
	}
}
