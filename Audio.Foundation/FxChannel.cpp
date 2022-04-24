#include "pch.h"
//#include "FxChannel.h"
//#include <functional>
//#include <algorithm>
//
//using namespace std;
//
//FxChannel::FxChannel(int iSamples)
//	: ::SampleBuffer(iSamples)
//{
//}
//
//void FxChannel::Receive(ISampleChannel& inputBuffer)
//{
//	ISampleChannel* pForwardBuffer = this;
//
//	if(m_vecFx.empty())	// input == output
//		pForwardBuffer = &inputBuffer;
//	else
//	{
//		ISampleBuffer* pSampleBuffer = inputBuffer.SampleBuffer;
//
//		if(NULL != pSampleBuffer)
//		{
//			const float* pSource = pSampleBuffer->SamplePtr;
//			float* pDestination = this->SamplePtr;
//			int iSamplesMax = this->SampleCount;
//
//			for_each(m_vecFx.begin(), m_vecFx.end(), [pDestination, pSource, iSamplesMax](IEffect* pFx) 
//			{ 
//				pFx->Apply(pSource, pDestination, iSamplesMax); 
//			});
//		}
//	}
//}
//
