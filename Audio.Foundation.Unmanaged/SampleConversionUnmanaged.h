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
			const double PanCenter = 0.5;
			const double PanLeft = 0.0;
			const double PanRight = +1.0;

			const double PanLaw3dB = 0.7079457843841379;
			const double PanLaw4Dot5dB = 0.5956621435290105;
			const double PanLaw6dB = 0.5011872336272722;

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
				static inline void LevelAndPanFactor(double level, double pan, double& factorLeft, double& factorRight, double panLaw = PanLaw4Dot5dB)
				{
					factorLeft = level * pow((1.0 - pan), panLaw);
					factorRight = level * pow(pan, panLaw);
				}

				/// <summary>
				/// Converts a 32-bit integer sample to an internal <see cref="sample"/>
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline Sample Int32ToSample(int value)
				{
					return value * Int32ToSampleFactor;
				}

				/// <summary>
				/// Converts an internal <see cref="sample"/> to 32-bit integer
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Integer sample</returns>
				static inline int SampleToInt32(Sample value)
				{
					return static_cast<int>(value * SampleToInt32Factor);
				}

				/// <summary>
				/// Converts a 24-bit integer sample to an internal <see cref="sample"/>
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline Sample Int24ToSample(int value)
				{
					return value * Int24ToSampleFactor;
				}

				/// <summary>
				/// Converts an internal <see cref="sample"/> to 24-bit integer
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Integer sample</returns>
				static inline int SampleToInt24(Sample value)
				{
					return static_cast<int>(value * SampleToInt24Factor);
				}

				/// <summary>
				/// Converts a 16-bit integer sample to an internal sample
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline Sample Int16ToSample(short value)
				{
					return value * Int16ToSampleFactor;
				}

				/// <summary>
				/// Converts an internal sample to a 16-bit integer
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline short SampleToInt16(Sample value)
				{
					return static_cast<short>(value * SampleToInt16Factor);
				}

				/// <summary>
				/// Converts a 32-bit float to an internal sample
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline Sample Float32ToSample(float value)
				{
					return static_cast<Sample>(value);
				}

				/// <summary>
				/// Converts an internal sample to a 32-bit float
				/// </summary>
				/// <param name="value">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline float SampleToFloat32(Sample value)
				{
					return static_cast<float>(value);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 32-bit int

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					32-bit int little endian sample
				*/
				static inline int SampleToSaturatedInt32(Sample value, Sample maxValue)
				{
					return static_cast<int>(Saturated(value, maxValue) * SampleToInt32Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 24-bit int

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					24-bit int little endian sample
				*/
				static inline int SampleToSaturatedInt24(Sample value, Sample maxValue)
				{
					return static_cast<int>(Saturated(value, maxValue) * SampleToInt24Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 16-bit int

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					16-bit int little endian sample
				*/
				static inline int SampleToSaturatedInt16(Sample value, Sample maxValue)
				{
					return static_cast<int>(Saturated(value, maxValue) * SampleToInt16Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from internal sample to 32-bit floaat

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					32-bit float little endian sample
				*/
				static inline float SampleToSaturatedFloat32(Sample value, Sample maxValue)
				{
					return static_cast<float>(Saturated(value, maxValue));
				}

				/*! \brief Basic saturation

					\param[in] value		internal sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					Saturated internal sample
				*/
				static inline Sample Saturated(Sample value, Sample maxValue)
				{
					return fabsl(value + maxValue) - fabsl(value - maxValue);
				}

				/// <summary>
				/// Adds two signals
				/// </summary>
				/// <param name="first">First signal</param>
				/// <param name="second">Second signal</param>
				/// <returns>Sum of <paramref name="first"/> and <paramref name="second"/> signal</returns>
				static inline Sample AddSignals(Sample first, Sample second)
				{
					return first + second;
				}


				static void ContinueRMSSumUp(Sample* pSamples, int iSamplesMax, double& sumUp)
				{
					for (int i = 0; i < iSamplesMax; i++)
					{
						Sample value = *pSamples++;
						sumUp += value * value;
					}
				}

				static double DbFullScaleRMS(Sample* pSamples, int iSamplesMax)
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
