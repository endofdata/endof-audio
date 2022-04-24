#pragma once
#include "pch.h"

namespace Audio
{
	namespace Wasapi
	{
		ref class Device;

		public ref class DeviceEnumerator : public System::Collections::Generic::IEnumerable<Device^>
		{
		public:
			DeviceEnumerator();
			virtual ~DeviceEnumerator();
			!DeviceEnumerator();

			property Device^ DefaultAudioEndpoint
			{
				Device^ get();
			}

			virtual System::Collections::Generic::IEnumerator<Device^>^ GetEnumerator() = System::Collections::Generic::IEnumerable<Device^>::GetEnumerator;

			virtual System::Collections::IEnumerator^ GetEnumeratorNonGeneric() = System::Collections::IEnumerable::GetEnumerator;

		private:
			void CleanUp();

			IMMDeviceEnumerator* m_pUnmanaged;
		};
	}
}
