#pragma once
#include "pch.h"
#include "OutputOfTSampleChannelPair.h"
#include "SampleContainer.h"
#include "SampleSharer.h"
#include "IInputChannel.h"
#include "ObjectFactory.h"

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
				class InputOfTSampleChannel : public IInputChannel
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
							throw gcnew AsioCoreException("InputChannel: Buffer pointers must not be NULL.", E_INVALIDARG);

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
						put_DirectMonitor(NULL);
						m_pContainer->Release();
						m_pSharer->Release();
					}

					virtual void Swap(bool readSecondHalf)
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

							int sampleCount = m_pContainer->SampleCount;
							float* pDest = m_pContainer->Channels[0]->SamplePtr;

							for (int i = 0; i < sampleCount; i++)
							{
								*pDest = ReadSample(pSource);
							}
						}
					}

					virtual void Send()
					{
						m_pSharer->RouteToTargets();
					}

					virtual bool get_IsActive()
					{
						return m_pContainer->IsActive;
					}

					virtual void put_IsActive(bool value)
					{
						m_pContainer->IsActive = value;
					}

					virtual IOutputChannelPair* get_DirectMonitor()
					{
						return m_pDirectMonitor;
					}

					virtual void put_DirectMonitor(IOutputChannelPair* value)
					{
						if (value != NULL)
						{
							value->AddRef();
						}

						IOutputChannelPair* pMonitor = (IOutputChannelPair*)InterlockedExchangePointer((void**)&m_pDirectMonitor, value);

						if (pMonitor != NULL)
						{
							pMonitor->Release();
						}						
					}

					virtual int get_SampleType()
					{
						return SAMPLE_TYPE;
					}

					virtual ISampleSharer& get_SampleSharer()
					{
						return *m_pSharer;
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

					IOutputChannelPair* m_pDirectMonitor;

					// was derived from...
					ISampleContainer* m_pContainer;
					ISampleSharer* m_pSharer;

					unsigned long m_refCount;
				};
			}
		}
	}
}



