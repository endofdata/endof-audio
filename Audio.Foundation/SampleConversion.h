#pragma once

#include <math.h>

namespace Audio
{
	namespace Foundation
	{

		delegate int BufferConverter(array<System::Byte>^ rawBytes, int count, array<System::Byte>^ output);
		delegate float SampleReader();
		delegate void SampleWriter(float sample);

		class SampleConversion
		{
		public:
			static int Int32ToSampleConverter(array<System::Byte>^ buffer, int count, array<System::Byte>^ output);
			static int Int16ToSampleConverter(array<System::Byte>^ buffer, int count, array<System::Byte>^ output);
			static int SampleToFloatConverter(array<System::Byte>^ buffer, int count, array<System::Single>^ output);
			static int IdentityConverter(int sampleSize, array<System::Byte>^ buffer, int count, array<System::Byte>^ output);
		};
	}
}
