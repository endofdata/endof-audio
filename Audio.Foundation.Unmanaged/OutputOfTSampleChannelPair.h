#pragma once

#include "IOutputChannelPair.h"
#include "ISampleProcessor.h"
#include "UnknownBase.h"
#include "ObjectFactory.h"

using namespace Audio::Foundation::Unmanaged;
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
				class OutputOfTSampleChannelPair : public IOutputChannelPair
				{
				public:
					/*! \brief Constructor

						Remembers the output buffer for converted sample data and initializes buffer positions.

						\param[in] pBuffer		Output buffer for TSample samples
						\param[in] iSamples		Size of buffer in samples
					*/
					OutputOfTSampleChannelPair(int idAsioChannelLeft, TSample* pBufferLeftA, TSample* pBufferLeftB,
						int idAsioChannelRight, TSample* pBufferRightA, TSample* pBufferRightB, int sampleCount) :
						m_asioChannelIdLeft(idAsioChannelLeft),
						m_pOutputLeftA(pBufferLeftA),
						m_pOutputLeftB(pBufferLeftB),
						m_asioChannelIdRight(idAsioChannelRight),
						m_pOutputRightA(pBufferRightA),
						m_pOutputRightB(pBufferRightB),
						m_sampleCount(sampleCount),
						m_writeSecondHalf(true),
						m_refCount(0)
					{
						if (nullptr == pBufferLeftA || nullptr == pBufferLeftB || nullptr == pBufferRightA || nullptr == pBufferRightB)
							throw std::invalid_argument("OutputChannelPair: Buffer pointers must not be nullptr.");
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
					virtual void OnNextBuffer(ISampleContainerPtr& container,  bool writeSecondHalf, int firstChannel)
					{
						if (IsActive)
						{
							int sampleChannels = container == nullptr ? 0 : container->ChannelCount;

							if (sampleChannels > firstChannel)
							{
								int secondChannel = std::min(sampleChannels - 1, firstChannel + 1);
								const Sample* pSourceLeft = container->Channels[firstChannel]->SamplePtr;
								const Sample* pSourceRight = container->Channels[secondChannel]->SamplePtr;

								TSample* pTargetLeft;
								TSample* pTargetRight;

								SelectBufferPointer(writeSecondHalf, pTargetLeft, pTargetRight);

								for (int i = 0; i < m_sampleCount; i++)
								{
									WriteSample(*pSourceLeft++, pTargetLeft);
									WriteSample(*pSourceRight++, pTargetRight);
								}
							}
						}
					}

					virtual void DirectOut(bool writeSecondHalf, void* pBuffer, bool fLeft, bool fRight)
					{
						TSample* pTargetLeft;
						TSample* pTargetRight;

						SelectBufferPointer(writeSecondHalf, pTargetLeft, pTargetRight);

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

					virtual bool get_IsActive()
					{
						return m_isActive;
					}

					virtual void put_IsActive(bool value)
					{
						m_isActive = value;
					}

					virtual int get_Id()
					{
						return m_asioChannelIdLeft << 16 | m_asioChannelIdRight;
					}

					TEMPLATED_IUNKNOWN

				protected:
					virtual bool GetInterface(const IID& riid, void** pResult)
					{
						if (riid == _uuidof(IUnknown))
						{
							*pResult = dynamic_cast<IUnknown*>(dynamic_cast<IOutputChannelPair*>(this));
							return true;
						}
						if (riid == _uuidof(IOutputChannelPair))
						{
							*pResult = dynamic_cast<IOutputChannelPair*>(this);
							return true;
						}
						if (riid == _uuidof(ISampleProcessor))
						{
							*pResult = dynamic_cast<ISampleProcessor*>(this);
							return true;
						}
						*pResult = nullptr;
						return false;
					}

				private:
					void SelectBufferPointer(bool writeSecondHalf, TSample*& pLeft, TSample*& pRight)
					{
						if (writeSecondHalf)
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

					virtual void WriteSample(Sample value, TSample*& pTarget) = 0;

					int m_asioChannelIdLeft;
					TSample* m_pOutputLeftA;
					TSample* m_pOutputLeftB;
					int m_asioChannelIdRight;
					TSample* m_pOutputRightA;
					TSample* m_pOutputRightB;
					int m_sampleCount;

					bool m_isActive;
					bool m_writeSecondHalf;
					ISampleProcessorPtr m_pNext;

					unsigned long m_refCount;
				};
			}
		}
	}
}
