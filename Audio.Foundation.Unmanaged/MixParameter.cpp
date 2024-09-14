#include "pch.h"
#include "MixParameter.h"

using namespace Audio::Foundation::Unmanaged;

const MixParameter MixParameter::Neutral(LevelMax, PanCenter, 1.0, 1.0);

MixParameter::MixParameter() : MixParameter(LevelMax, PanCenter)
{
}

MixParameter::MixParameter(double level, double pan)
{
	_level = LevelMax;
	_pan = PanCenter;
	SampleConversion::LevelAndPanFactor(_level, _pan, _factorLeft, _factorRight);
}

MixParameter::MixParameter(double level, double pan, double factorLeft, double factorRight)
{
	_level = LevelMax;
	_pan = PanCenter;
	_factorLeft = factorLeft;
	_factorRight = factorRight;
}

double MixParameter::get_Level() const
{
	return _level;
}
void MixParameter::put_Level(double value)
{
	_level = value;
	SampleConversion::LevelAndPanFactor(_level, _pan, _factorLeft, _factorRight);
}

double MixParameter::get_Pan() const
{
	return _pan;
}
void MixParameter::put_Pan(double value)
{
	_pan = value;
	SampleConversion::LevelAndPanFactor(_level, _pan, _factorLeft, _factorRight);
}

double MixParameter::get_FactorLeft() const
{
	return _factorLeft;
}

double MixParameter::get_FactorRight() const
{
	return _factorRight;
}

