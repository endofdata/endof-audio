#pragma once
#include "pch.h"
#include "OutputOfTSampleChannelPair.h"
#include "SampleContainer.h"
#include "SampleSharer.h"
#include "IInputChannel.h"
#include "ISampleSource.h"
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
				template<typename TSample, int SAMPLE_TYPE>
				class InputOfTSampleChannel : public ISampleSource, public IInputChannel
				{
				public:
					/*! \brief Constructor

					*/
					InputOfTSampleChannel(int iAsioChannel, TSample* pBufferA, TSample* pBufferB, int sampleCount) :
						m_iAsioChannel(iAsioChannel),
						m_pBufferA(pBufferA),
						m_pBufferB(pBufferB),
						m_pDirectMonitor(NULL),
						m_pContainer(NULL),
						m_pSharer(NULL),
						m_readSecondHalf(false),
						m_refCount(0)
					{
						if (NULL == m_pBufferA || NULL == m_pBufferB)
							throw std::invalid_argument("InputChannel: Buffer pointers must not be NULL.");

						ZeroMemory(m_pBufferA, sampleCount * sizeof(TSample));
						ZeroMemory(m_pBufferB, sampleCount * sizeof(TSample));

						m_pContainer = ObjectFactory::CreateSampleContainer(sampleCount, 1);
						m_pSharer = ObjectFactory::CreateSampleSharer();
						m_pSharer->Source = m_pContainer;
					}

					/*! \brief Destructor
					*/
					virtual ~InputOfTSampleChannel()
					{
					}

					virtual void Pull(bool readSecondHalf)
					{
						m_readSecondHalf = readSecondHalf;

						if (m_pContainer->IsActive)
						{
							TSample* pSource;
							SelectBufferPointer(pSource);

							// if monitor is attached, send to both monitor channels
							if (NULL != m_pDirectMonitor)
							{
								m_pDirectMonitor->DirectOut(pSource, true, true);
							}

							// Convert the incoming samples to float and write them to the container
							int sampleCount = m_pContainer->SampleCount;
							float* pDest = m_pContainer->Channels[0]->SamplePtr;

							for (int i = 0; i < sampleCount; i++)
							{
								*pDest = ReadSample(pSource);
							}
						}
					}

					virtual void Push()
					{
						// Push the samples form the container to all targets attached to the sharer
						m_pSharer->Push();
					}

					virtual bool get_IsActive()
					{
						return m_pContainer->IsActive;
					}

					virtual void put_IsActive(bool value)
					{
						m_pContainer->IsActive = value;
					}

					virtual IOutputChannelPairPtr get_DirectMonitor()
					{
						return m_pDirectMonitor;
					}

					virtual void put_DirectMonitor(IOutputChannelPairPtr value)
					{
						m_pDirectMonitor = value;
					}

					virtual int get_SampleType()
					{
						return SAMPLE_TYPE;
					}

					virtual ISampleSharerPtr get_SampleSharer()
					{
						return m_pSharer;
					}

					TEMPLATED_IUNKNOWN

				protected:
					virtual bool GetInterface(const IID& riid, void** pResult)
					{
						if (riid == _uuidof(IUnknown))
						{
							*pResult = dynamic_cast<IUnknown*>(this);
							return true;
						}
						if (riid == _uuidof(ISampleSource))
						{
							*pResult = dynamic_cast<ISampleSource*>(this);
							return true;
						}
						if (riid == _uuidof(IInputChannel))
						{
							*pResult = dynamic_cast<IInputChannel*>(this);
							return true;
						}
						*pResult = NULL;
						return false;
					}

				private:
					void SelectBufferPointer(TSample*& pSource)
					{
						pSource = m_readSecondHalf ? m_pBufferB : m_pBufferA;
					}

					virtual float ReadSample(TSample*& pSource) = 0;

					int m_iAsioChannel;
					TSample* m_pBufferA;
					TSample* m_pBufferB;
					bool m_readSecondHalf;

					IOutputChannelPairPtr m_pDirectMonitor;

					// was derived from...
					ISampleContainerPtr m_pContainer;
					ISampleSharerPtr m_pSharer;

					unsigned long m_refCount;
				};
			}
		}
	}
}



