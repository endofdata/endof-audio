#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <math.h>
#include <stdexcept>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			const double SampleMaxRange32 = 2147483647.0;
			const double Int32ToSampleFactor = (1.f / SampleMaxRange32);
			const double SampleToInt32Factor = SampleMaxRange32;

			const double SampleMaxRange16 = 32767.0;
			const double Int16ToSampleFactor = (1.0 / SampleMaxRange16);
			const double SampleToInt16Factor = SampleMaxRange16;

			const double LevelMax = 1.0;
			const double PanCenter = 0.0;
			const double PanLeft = -1.0;
			const double PanRight = +1.0;

			class _AUDIO_FOUNDATION_UNMANAGED_API SampleConversion
			{
			public:

				/// <summary>
				/// Calculates the factor by which samples for the left and right channel can be multiplied 
				/// to achieve the given <paramref name="level"/> and <paramref name="pan"/>.
				/// </summary>
				/// <param name="level">Level factor</param>
				/// <param name="pan">Pan position</param>
				/// <param name="factorLeft">Receives the factor for the left channel</param>
				/// <param name="factorRight">Receives the factor for the right channel</param>
				static inline void LevelAndPanFactor(double level, double pan, double& factorLeft, double& factorRight)
				{
					factorLeft = (pan + PanLeft) * -0.5f * level;
					factorRight = (pan + PanRight) * 0.5f * level;
				}

				/// <summary>
				/// Converts a 32-bit integer sample to an internal <see cref="sample"/>
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline sample Int32ToSample(int value)
				{
					return value * Int32ToSampleFactor;
				}

				/// <summary>
				/// Converts an internal <see cref="sample"/> to 32-bit integer
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Integer sample</returns>
				static inline int SampleToInt32(sample value)
				{
					return SaturatedConvert32(value, 1.0);
				}

				/// <summary>
				/// Converts a 16-bit integer sample to an internal sample
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline sample Int16ToSample(short value)
				{
					return value * Int16ToSampleFactor;
				}

				/// <summary>
				/// Converts an internal sample to a 16-bit integer
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline short SampleToInt16(sample value)
				{
					return SaturatedConvert16(value, 1.0);
				}

				/// <summary>
				/// Converts a 32-bit float to an internal sample
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline sample SampleToFloat(float value)
				{
					return (sample)value;
				}

				/// <summary>
				/// Converts an internal sample to a 32-bit float
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline float SampleToFloat(sample value)
				{
					return (float)value;
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to int 32

					The original implementation did only saturation, integer conversion within the
					same method was added by myself.

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					Int32 little endian sample
				*/
				static inline int SaturatedConvert32(sample value, sample maxValue)
				{
					return (int)((fabsl(value + maxValue) - fabsl(value - maxValue)) * SampleToInt32Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to int 16

					The original implementation did only saturation, integer conversion within the
					same method was added by myself.

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					Int16 little endian sample
				*/
				static inline int SaturatedConvert16(sample value, sample maxValue)
				{
					return (int)((fabsl(value + maxValue) - fabsl(value - maxValue)) * SampleToInt16Factor);
				}

				/// <summary>
				/// Adds two signals
				/// </summary>
				/// <param name="first">First signal</param>
				/// <param name="second">Second signal</param>
				/// <returns>Sum of <paramref name="first"/> and <paramref name="second"/> signal</returns>
				static inline sample AddSignals(sample first, sample second)
				{
					return first + second;
				}


				static void ContinueRMSSumUp(sample* pSamples, int iSamplesMax, double& sumUp)
				{
					for (int i = 0; i < iSamplesMax; i++)
					{
						sample sample = *pSamples++;
						sumUp += sample * sample;
					}
				}

				static double DbFullScaleRMS(sample* pSamples, int iSamplesMax)
				{
					double sumUp = 0.0f;
					ContinueRMSSumUp(pSamples, iSamplesMax, sumUp);
					return DbFullScaleRMS(sumUp, iSamplesMax);
				}

				static double DbFullScaleRMS(double sumUp, int iSamples)
				{
					if (iSamples == 0)
						throw new std::invalid_argument("Parameter 'iSamples' cannot be 0.");

					double rms = sqrt(sumUp / iSamples);
					return 20.0f * log10(rms);
				}

				static inline float SamplesToMilliSeconds(int iSamples, int sampleRate)
				{
					if (sampleRate == 0)
						throw new std::invalid_argument("Parameter 'sampleRate' cannot be 0.");

					return iSamples / ((float)sampleRate / 1000.0f);
				}

				static inline float MilliSecondsToSamples(int iMilliSeconds, int sampleRate)
				{
					return ((float)sampleRate / 1000.0f) * iMilliSeconds;
				}

			};
		}
	}
}
