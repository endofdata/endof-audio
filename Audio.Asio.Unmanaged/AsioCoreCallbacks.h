#pragma once

#include <Audio.Asio.Unmanaged.h>

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			class AsioCore;

			class _AUDIO_ASIO_UNMANAGED_API AsioCoreCallbacks
			{
			public:
				static AsioCoreCallbacks& Create(AsioCore* pCore);

				virtual ~AsioCoreCallbacks();

				ASIOCallbacks& get_Callbacks();

				_declspec(property(get = get_Callbacks)) ASIOCallbacks& Callbacks;

				static const int MaxDeviceCount;

			private:
				AsioCore* m_pCore;
				AsioCoreCallbacks(AsioCore* pCore, ASIOCallbacks& callbacks, int deviceIndex);

				ASIOCallbacks m_callbacks;
				int m_deviceIndex;

				static AsioCoreCallbacks* s_UsedDevices[];

				static void BufferSwitchEntryDev0(long doubleBufferIndex, ASIOBool directProcess);
				static ASIOTime* BufferSwitchTimeInfoEntryDev0(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
				static void SampleRateDidChangeEntryDev0(ASIOSampleRate rate);
				static long AsioMessageEntryDev0(long selector, long value, void* message, double* opt);

				static void BufferSwitchEntryDev1(long doubleBufferIndex, ASIOBool directProcess);
				static ASIOTime* BufferSwitchTimeInfoEntryDev1(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
				static void SampleRateDidChangeEntryDev1(ASIOSampleRate rate);
				static long AsioMessageEntryDev1(long selector, long value, void* message, double* opt);

				static void BufferSwitchEntryDev2(long doubleBufferIndex, ASIOBool directProcess);
				static ASIOTime* BufferSwitchTimeInfoEntryDev2(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess);
				static void SampleRateDidChangeEntryDev2(ASIOSampleRate rate);
				static long AsioMessageEntryDev2(long selector, long value, void* message, double* opt);
			};
		}
	}
}