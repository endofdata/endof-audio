#pragma once

#include <math.h>
#include <Audio.Foundation.Unmanaged.h>

namespace Audio
{
	namespace Foundation
	{

		delegate int BufferConverter(array<System::Byte>^ rawBytes, int count, array<System::Byte>^ output);
		delegate sample SampleReader();
		delegate void SampleWriter(sample sample);

		class SampleConversion
		{
		public:
			static int Int32ToSampleConverter(array<System::Byte>^ buffer, int count, array<System::Byte>^ output);
			static int Int16ToSampleConverter(array<System::Byte>^ buffer, int count, array<System::Byte>^ output);
			static int Float32ToSampleConverter(array<System::Byte>^ buffer, int count, array<System::Byte>^ output);
			static int IdentityConverter(int sampleSize, array<System::Byte>^ buffer, int count, array<System::Byte>^ output);

			static int SampleToFloat32Converter(array<System::Byte>^ buffer, int count, array<System::Single>^ output);
		};
	}
}
