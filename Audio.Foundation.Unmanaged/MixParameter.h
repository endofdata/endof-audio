#pragma once
#include <Audio.Foundation.Unmanaged.h>
#include <SampleConversionUnmanaged.h>
#include <functional>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			struct _AUDIO_FOUNDATION_UNMANAGED_API MixParameter
			{
			public:
				static const MixParameter Neutral;

				MixParameter();
				MixParameter(double level, double pan);
				MixParameter(double level, double pan, double factorLeft, double factorRight);

				double get_Level() const;
				void put_Level(double value);
				_declspec(property(get = get_Level, put = put_Level)) double Level;

				double get_Pan() const;
				void put_Pan(double value);
				_declspec(property(get = get_Pan, put = put_Pan)) double Pan;

				double get_FactorLeft() const;
				_declspec(property(get = get_FactorLeft)) double FactorLeft;

				double get_FactorRight() const;
				_declspec(property(get = get_FactorRight)) double FactorRight;

			private:
				double _level;
				double _pan;
				double _factorLeft;
				double _factorRight;
			};
		}
	}
}