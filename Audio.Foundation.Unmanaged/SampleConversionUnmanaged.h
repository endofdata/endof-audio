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
			const float FloatMaxRange32 = 2147483647.f;
			//const float FloatMaxRange = 1073741823.f;
			const float Int32ToFloatFactor = (1.f / FloatMaxRange32);
			//const float FloatToIntFactor = (FloatMaxRange * 0.5f);
			const float FloatToInt32Factor = FloatMaxRange32;

			const float FloatMaxRange16 = 32767.f;
			const float Int16ToFloatFactor = (1.f / FloatMaxRange16);
			const float FloatToInt16Factor = FloatMaxRange16;

			const float LevelMax = 1.0f;
			const float PanCenter = 0.0f;
			const float PanLeft = -1.0f;
			const float PanRight = +1.0f;

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
				static inline void LevelAndPanFactor(float level, float pan, float& factorLeft, float& factorRight)
				{
					factorLeft = (pan + PanLeft) * -0.5f * level;
					factorRight = (pan + PanRight) * 0.5f * level;
				}

				/// <summary>
				/// Converts a 32-bit integer sample to a float sample
				/// </summary>
				/// <param name="sample">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline float Int32ToFloat(int sample)
				{
					return sample * Int32ToFloatFactor;
				}

				/// <summary>
				/// Converts a float sample to 32-bit integer
				/// </summary>
				/// <param name="sample">Sample to convert</param>
				/// <returns>Integer sample</returns>
				static inline int FloatToInt32(float sample)
				{
					return SaturatedConvert32(sample, 1.0);
				}

				/// <summary>
				/// Converts a 16-bit integer sample to a float sample
				/// </summary>
				/// <param name="sample">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline float Int16ToFloat(short sample)
				{
					return sample * Int16ToFloatFactor;
				}

				/// <summary>
				/// Converts a 16-bit integer sample to a float sample
				/// </summary>
				/// <param name="sample">Sample to convert</param>
				/// <returns>Float sample</returns>
				static inline short FloatToInt16(float sample)
				{
					return SaturatedConvert16(sample, 1.0);
				}

				/*! \brief A helper, which adds saturation during sample conversion from float to int 32

					The original implementation did only saturation, integer conversion within the
					same method was added by myself.

					\param[in] sample		32-bit float sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					Int32 little endian sample
				*/
				static inline int SaturatedConvert32(float sample, float maxValue)
				{
					return (int)((fabsf(sample + maxValue) - fabsf(sample - maxValue)) * FloatToInt32Factor);
				}

				/*! \brief A helper, which adds saturation during sample conversion from float to int 16

					The original implementation did only saturation, integer conversion within the
					same method was added by myself.

					\param[in] sample		16-bit float sample to convert
					\param[in] maxValue		Maximum sample value (usually 1.0f)
					\return					Int16 little endian sample
				*/
				static inline int SaturatedConvert16(float sample, float maxValue)
				{
					return (int)((fabsf(sample + maxValue) - fabsf(sample - maxValue)) * FloatToInt16Factor);
				}

				/// <summary>
				/// Adds two signals
				/// </summary>
				/// <param name="first">First signal</param>
				/// <param name="second">Second signal</param>
				/// <returns>Sum of <paramref name="first"/> and <paramref name="second"/> signal</returns>
				static inline float AddSignals(float first, float second)
				{
					return first + second;
				}


				static void ContinueRMSSumUp(float* pSamples, int iSamplesMax, double& sumUp)
				{
					for (int i = 0; i < iSamplesMax; i++)
					{
						float sample = *pSamples++;
						sumUp += sample * sample;
					}
				}

				static double DbFullScaleRMS(float* pSamples, int iSamplesMax)
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
