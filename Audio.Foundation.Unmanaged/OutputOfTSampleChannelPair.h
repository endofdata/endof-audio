#pragma once

#include "IOutputChannelPair.h"
#include "ISampleReceiver.h"
#include "UnknownBase.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Templates
			{
				template <typename TSample, int SAMPLE_TYPE, size_t SAMPLE_SIZE>
				class OutputOfTSampleChannelPair : public ISampleReceiver, public IOutputChannelPair
				{
				public:
					/*! \brief Constructor

						Remembers the output buffer for converted sample data and initializes buffer positions.

						\param[in] pBuffer		Output buffer for TSample samples
						\param[in] iSamples		Size of buffer in samples
					*/
					OutputOfTSampleChannelPair(int iAsioChannelLeft, TSample* pBufferLeftA, TSample* pBufferLeftB,
						int iAsioChannelRight, TSample* pBufferRightA, TSample* pBufferRightB, int sampleCount) :
						m_iAsioChannelLeft(iAsioChannelLeft),
						m_pOutputLeftA(pBufferLeftA),
						m_pOutputLeftB(pBufferLeftB),
						m_iAsioChannelRight(iAsioChannelRight),
						m_pOutputRightA(pBufferRightA),
						m_pOutputRightB(pBufferRightB),
						m_sampleCount(sampleCount),
						m_writeSecondHalf(true),
						m_refCount(0)
					{
						if (NULL == pBufferLeftA || NULL == pBufferLeftB || NULL == pBufferRightA || NULL == pBufferRightB)
							throw gcnew AsioCoreException("OutputChannelPair: Buffer pointers must not be NULL.", E_INVALIDARG);
					}

					/*! \brief Destructor
					*/
					virtual ~OutputOfTSampleChannelPair()
					{
					}

					/*! \brief Sets the current write position to the indicated buffer half

						The output buffer is big enough to hold 2 * SampleCount, where
						SampleCount is the constructor argument. When the ASIO driver signals
						a buffer switch, this method is used to update the current write
						position to either the first or the second buffer half.

						\param[in] writeSecondHalf	If true, the second buffer half gets activated.
					*/
					virtual void Swap(bool writeSecondHalf)
					{
						m_writeSecondHalf = writeSecondHalf;
					}

					virtual void Flush()
					{
					}

					virtual void Receive(ISampleContainer& input)
					{
						float* pSourceLeft = input.LeftChannel->SamplePtr;
						float* pSourceRight = input.RightChannel->SamplePtr;

						TSample* pTargetLeft;
						TSample* pTargetRight;

						SelectBufferPointer(pTargetLeft, pTargetRight);

						for (int i = 0; i < m_sampleCount; i++)
						{
							WriteSample(*pSourceLeft++, pTargetLeft);
							WriteSample(*pSourceRight++, pTargetRight);
						}
					}


					//int get_SampleCount();

					virtual void DirectOut(void* pBuffer, bool fLeft, bool fRight)
					{
						TSample* pTargetLeft;
						TSample* pTargetRight;

						SelectBufferPointer(pTargetLeft, pTargetRight);

						if (fLeft)
						{
							CopyMemory(pTargetLeft, pBuffer, m_sampleCount * SAMPLE_SIZE);
						}
						if (fRight)
						{
							CopyMemory(pTargetRight, pBuffer, m_sampleCount * SAMPLE_SIZE);
						}
					}

					virtual int get_SampleType()
					{
						return SAMPLE_TYPE;
					}

					TEMPLATED_IUNKNOWN

				protected:
					virtual bool GetInterface(const IID& riid, void** pResult)
					{
						if (riid == _uuidof(IUnknown))
						{
							*pResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleReceiver*>(this));
							return true;
						}
						if (riid == _uuidof(ISampleReceiver))
						{
							*pResult = dynamic_cast<ISampleReceiver*>(this);
							return true;
						}
						if (riid == _uuidof(IOutputChannelPair))
						{
							*pResult = dynamic_cast<IOutputChannelPair*>(this);
							return true;
						}
						*pResult = NULL;
						return false;
					}

				private:
					void SelectBufferPointer(TSample*& pLeft, TSample*& pRight)
					{
						if (m_writeSecondHalf)
						{
							pLeft = m_pOutputLeftB;
							pRight = m_pOutputRightB;
						}
						else
						{
							pLeft = m_pOutputLeftA;
							pRight = m_pOutputRightA;
						}
					}

					virtual void WriteSample(float value, TSample*& pTarget) = 0;

					int m_iAsioChannelLeft;
					TSample* m_pOutputLeftA;
					TSample* m_pOutputLeftB;
					int m_iAsioChannelRight;
					TSample* m_pOutputRightA;
					TSample* m_pOutputRightB;
					int m_sampleCount;
					bool m_writeSecondHalf;
					//int m_iSamples;

					unsigned long m_refCount;
				};
			}
		}
	}
}
