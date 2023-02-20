#pragma once
#include "pch.h"
#include "OutputOfTSampleChannelPair.h"
#include "SampleContainer.h"
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
				class InputOfTSampleChannel : public IInputChannel, public ISampleSource
				{
				public:
					/*! \brief Constructor

					*/
					InputOfTSampleChannel(int iAsioChannel, TSample* pBufferA, TSample* pBufferB, int sampleCount) :
						m_iAsioChannel(iAsioChannel),
						m_pBufferA(pBufferA),
						m_pBufferB(pBufferB),
						m_pDirectMonitor(nullptr),
						m_pContainer(nullptr),
						m_readSecondHalf(false),
						m_isActive(false),
						m_pFirst(nullptr),
						m_refCount(0)
					{
						if (nullptr == m_pBufferA || nullptr == m_pBufferB)
							throw std::invalid_argument("InputChannel: Buffer pointers must not be nullptr.");

						m_pContainer = ObjectFactory::CreateSampleContainer(sampleCount, 1);
					}

					/*! \brief Destructor
					*/
					virtual ~InputOfTSampleChannel()
					{
					}

					virtual void OnNextBuffer(bool readSecondHalf)
					{
						m_readSecondHalf = readSecondHalf;

						if (IsActive && HasFirst)
						{
							TSample* pSource;
							SelectBufferPointer(pSource);

							// if monitor is attached, send to both monitor channels
							if (nullptr != m_pDirectMonitor)
							{
								m_pDirectMonitor->DirectOut(pSource, true, true);
							}

							// Convert the incoming samples to internal and write them to the container
							int sampleCount = m_pContainer->SampleCount;
							Sample* pDest = m_pContainer->Channels[0]->SamplePtr;

							for (int i = 0; i < sampleCount; i++)
							{
								*pDest++ = ReadSample(pSource);
							}
							m_pFirst->Process(m_pContainer);
						}
					}

					virtual ISampleContainerPtr get_Container()
					{
						return m_pContainer;
					}

					virtual bool get_IsActive()
					{
						return m_isActive;
					}

					virtual void put_IsActive(bool value)
					{
						m_isActive = value;
					}

					virtual bool get_SupportsDirectMonitor()
					{
						return true;
					}

					virtual IOutputChannelPairPtr get_DirectMonitor()
					{
						return m_pDirectMonitor;
					}

					virtual void put_DirectMonitor(IOutputChannelPairPtr value)
					{
						m_pDirectMonitor = value;
					}

					virtual bool get_HasDirectMonitor()
					{
						return m_pDirectMonitor != nullptr;
					}

					virtual int get_SampleType()
					{
						return SAMPLE_TYPE;
					}

					virtual ISampleProcessorPtr get_First()
					{
						return m_pFirst;
					}

					virtual void put_First(ISampleProcessorPtr value)
					{
						m_pFirst = value;
					}

					virtual bool get_HasFirst()
					{
						return m_pFirst != nullptr;
					}

					TEMPLATED_IUNKNOWN

				protected:
					virtual bool GetInterface(const IID& riid, void** pResult)
					{
						if (riid == _uuidof(IUnknown))
						{
							*pResult = dynamic_cast<IUnknown*>(dynamic_cast<IInputChannel*>(this));
							return true;
						}
						if (riid == _uuidof(IInputChannel))
						{
							*pResult = dynamic_cast<IInputChannel*>(this);
							return true;
						}
						if (riid == _uuidof(ISampleSource))
						{
							*pResult = dynamic_cast<ISampleSource*>(this);
							return true;
						}
						*pResult = nullptr;
						return false;
					}

				private:
					void SelectBufferPointer(TSample*& pSource)
					{
						pSource = m_readSecondHalf ? m_pBufferB : m_pBufferA;
					}

					virtual Sample ReadSample(TSample*& pSource) = 0;

					int m_iAsioChannel;
					TSample* m_pBufferA;
					TSample* m_pBufferB;
					bool m_readSecondHalf;

					bool m_isActive;
					IOutputChannelPairPtr m_pDirectMonitor;
					ISampleContainerPtr m_pContainer;
					ISampleProcessorPtr m_pFirst;

					unsigned long m_refCount;
				};
			}
		}
	}
}



