#include "pch.h"


	/*
	    // ASIO -> VST
    void ToFloat(long *src, float *target, long frames)
    {
    while (--frames >= 0)
      *target++ = *src++ * (1.f / 2147483647.f);
    }

    // little helper to keep output in range (hard clipping)
    static inline float Saturate(float input, float fMax)
    {
    float x1 = fabsf(input + fMax);
    float x2 = fabsf(input - fMax);
    return 0.5f * (x1 - x2);
    }

    // convert VST -> ASIO
    void ToLong(float *source, long *dst, long frames)
    {
     // the basic algorithm would be:
     //  while (--frames >= 0)
     //    *dst++ = *source++ * 2147483647.f;
    //   but that would allow nasty buzzing in case of overflows
    while (--frames >= 0)
      *dst++ = (long)(Saturate(*source++, 1.f) * 2147483647.f);
    }
*/



//void ClsIdFromString()
//{
	// convert from managed string to unmanaged chaos string
	//pinm_ptr<const wcharm_t> clsid = PtrToStringChars(installedDriver->ClsId);

	// convert string from registry to CLSID
	//CLSIDFromString((LPOLESTR)clsid, &m_clsid);
//}

//  static inline float Saturate(float input, float fMax)
//  {
	//return 0.5f * (fabsf(input + fMax) - fabsf(input - fMax));
//  }


// dbFS from RMS
//double sum = 0;
//for (var i = 0; i < _buffer.length; i = i + 2)
//{
//    double sample = BitConverter.ToInt16(_buffer, i) / 32768.0;
//    sum += (sample * sample);
//}
//double rms = Math.Sqrt(sum / (_buffer.length / 2));
//var decibel = 20 * Math.Log10(rms);

