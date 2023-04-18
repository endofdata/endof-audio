#pragma once

#include <Audio.Asio.Unmanaged.h>

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#ifndef interface
#define interface __interface
#endif
#include <asio.h>
#include <iasiodrv.h>
#include <vector>
#include <IInputChannel.h>
#include <IOutputChannelPair.h>
#include <IProcessingChain.h>
#include <memory>
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

				static inline std::shared_ptr<AsioCore> CreateInstancePtr(REFCLSID clsid)
				{
					return std::shared_ptr<AsioCore>(CreateInstance(clsid), [](AsioCore* pDevice) { DeleteInstance(pDevice); });
				}

				static AsioCore* CreateInstance(REFCLSID clsid);
				static void DeleteInstance(AsioCore* pDevice);

				virtual ~AsioCore();

				void Start();

				void Stop();

				void ShowControlPanel();

				void SetInputMonitoring(int iInputChannel, int iOutputPair);

				void GetDriverName(char* pcOut);

				int GetDriverVersion();

				int GetHardwareInputName(int index, char* pcOut, int max);

				int GetHardwareOutputName(int index, char* pcOut, int max);

				_declspec(property(get = get_HardwareInputCount)) int HardwareInputCount;

				int get_HardwareInputCount();

				_declspec(property(get = get_HardwareOutputCount)) int HardwareOutputCount;

				int get_HardwareOutputCount();

				_declspec(property(get = get_ProcessingChain)) IProcessingChainPtr ProcessingChain;

				IProcessingChainPtr& get_ProcessingChain();

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

				void CreateBuffers(const int inputChannelIds[], int numInputIds, const int outputChannelIds[], int numOutputIds, int sampleCount, float outputSaturation = 0.0f);

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

				void CreateOutputChannels(int offset, int count, float saturation = 0.0f);

				int MapSampleType(ASIOSampleType asioSampleType);

				void DisposeBuffers();

				void OnBufferSwitch(long doubleBufferIndex, ASIOBool directProcess) const;

				ASIOTime* OnBufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);

				void OnSampleRateDidChange(ASIOSampleRate rate);

				long OnAsioMessage(long selector, long value, void* message, double* opt);

				void ThrowIfFailed(ASIOError error);

				int GetHardwareChannelName(ASIOBool isInput, int index, char* pcOut, int max);

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

				IProcessingChainPtr m_processingChain;

				int m_iCurrentMonitorInput;

				ASIOSampleRate m_sampleRate;

				ASIOSampleType m_sampleType;

				BufferSwitchEventHandler m_bufferSwitchEventHandler;

				ASIOError m_lastError;
				AsioCoreCallbacks* m_pCoreCallbacks;

				friend class AsioCoreCallbacks;
			};

			typedef std::shared_ptr<AsioCore> AsioCorePtr;
		}
	}
}
