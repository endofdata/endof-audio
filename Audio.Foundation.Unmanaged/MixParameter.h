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
				MixParameter() : MixParameter(LevelMax, PanCenter)
				{
				}

				MixParameter(double level, double pan)
				{
					_level = LevelMax;
					_pan = PanCenter;
					SampleConversion::LevelAndPanFactor(_level, _pan, _factorLeft, _factorRight);
				}

				double get_Level() const
				{
					return _level;
				}
				void put_Level(double value)
				{
					_level = value;
					SampleConversion::LevelAndPanFactor(_level, _pan, _factorLeft, _factorRight);
				}
				_declspec(property(get = get_Level, put = put_Level)) double Level;

				double get_Pan() const
				{
					return _pan;
				}
				void put_Pan(double value)
				{
					_pan = value;
					SampleConversion::LevelAndPanFactor(_level, _pan, _factorLeft, _factorRight);
				}
				_declspec(property(get = get_Pan, put = put_Pan)) double Pan;

				double get_FactorLeft() const
				{
					return _factorLeft;
				}
				_declspec(property(get = get_FactorLeft)) double FactorLeft;

				double get_FactorRight() const
				{
					return _factorRight;
				}
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