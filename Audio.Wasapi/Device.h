#pragma once
#include "pch.h"

namespace Audio
{
	namespace Wasapi
	{
		ref class AudioClient;

		public ref class Device
		{
		internal:
			Device(IMMDevice* pUnmanaged);
			virtual ~Device();
			!Device();

			AudioClient^ Activate();

		private:
			void CleanUp();

			IMMDevice* m_pUnmanaged;
		};
	}
}