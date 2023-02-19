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
			const double Int32ToSampleFactor = (1.0 / SampleMaxRange32);
			const double SampleToInt32Factor = SampleMaxRange32;

			const double SampleMaxRange24 = 49150.5;
			const double Int24ToSampleFactor = (1.0 / SampleMaxRange24);
			const double SampleToInt24Factor = SampleMaxRange24;

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
					return value * SampleToInt32Factor;
				}

				/// <summary>
				/// Converts a 24-bit integer sample to an internal <see cref="sample"/>
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline sample Int24ToSample(int value)
				{
					return value * Int24ToSampleFactor;
				}

				/// <summary>
				/// Converts an internal <see cref="sample"/> to 24-bit integer
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Integer sample</returns>
				static inline int SampleToInt24(sample value)
				{
					return value * SampleToInt24Factor;
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
					return value * SampleToInt16Factor;
				}

				/// <summary>
				/// Converts a 32-bit float to an internal sample
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline sample Float32ToSample(float value)
				{
					return static_cast<sample>(value);
				}

				/// <summary>
				/// Converts an internal sample to a 32-bit float
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline float SampleToFloat32(sample value)
				{
					return static_cast<float>(value);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 32-bit int

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					32-bit int little endian sample
				*/
				static inline int SaturatedConvertInt32(sample value, sample maxValue)
				{
					return static_cast<int>(Saturated(value, maxValue) * SampleToInt32Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 24-bit int

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					24-bit int little endian sample
				*/
				static inline int SaturatedConvertInt24(sample value, sample maxValue)
				{
					return static_cast<int>(Saturated(value, maxValue) * SampleToInt24Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 16-bit int

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					16-bit int little endian sample
				*/
				static inline int SaturatedConvertInt16(sample value, sample maxValue)
				{
					return static_cast<int>(Saturated(value, maxValue) * SampleToInt16Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 32-bit floaat

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					32-bit float little endian sample
				*/
				static inline float SaturatedConvertFloat32(sample value, sample maxValue)
				{
					return static_cast<float>(Saturated(value, maxValue));
				}

				/*! \brief Basic saturation

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					Saturated internal sample
				*/
				static inline sample Saturated(sample value, sample maxValue)
				{
					return fabsl(value + maxValue) - fabsl(value - maxValue);
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
