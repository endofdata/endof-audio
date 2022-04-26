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
						m_pMonitor(NULL),
						m_refCount(0)
					{
						if (NULL == m_pBufferA || NULL == m_pBufferB)
							throw gcnew AsioCoreException("InputChannel: Buffer pointers must not be NULL.", E_INVALIDARG);

						m_pContainer = ObjectFactory::CreateSampleContainer(sampleCount);
						m_pSharer = ObjectFactory::CreateSampleSharer();
					}

					/*! \brief Destructor
					*/
					virtual ~InputOfTSampleChannel()
					{
						put_Monitor(NULL);
						m_pContainer->Release();
						m_pSharer->Release();
					}

					virtual void Swap(bool readSecondHalf)
					{
						if (SampleContainer.IsActive)
						{
							TSample* pSource = readSecondHalf ? m_pBufferB : m_pBufferA;

							// if monitor is attached, send to both monitor channels
							if (NULL != m_pMonitor)
							{
								m_pMonitor->DirectOut(pSource, true, true);
							}
							float* pDestLeft = SampleContainer.LeftChannel->SamplePtr;
							float* pDestRight = SampleContainer.RightChannel->SamplePtr;

							// double the voulume to get full level on both channels
							float volPanFactorLeft = 0.0f;
							float volPanFactorRight = 0.0f;
							Audio::Foundation::Unmanaged::SampleConversion::VolumeAndPanFactor(2.0f, 0.0f, volPanFactorLeft, volPanFactorRight);

							int sampleCount = SampleContainer.SampleCount;
							for (int i = 0; i < sampleCount; i++)
							{
								float floatSample = ReadSample(pSource);

								*pDestLeft++ = floatSample * volPanFactorLeft;
								*pDestRight++ = floatSample * volPanFactorRight;
							}
						}
					}

					virtual void Send()
					{
						m_pSharer->RouteToSends();
					}

					virtual IOutputChannelPair* get_Monitor()
					{
						return m_pMonitor;
					}

					virtual void put_Monitor(IOutputChannelPair* value)
					{
						if (value != NULL)
						{
							value->AddRef();
						}

						IOutputChannelPair* pMonitor = (IOutputChannelPair*)InterlockedExchangePointer((void**)&m_pMonitor, value);

						if (pMonitor != NULL)
						{
							pMonitor->Release();
						}						
					}

					virtual int get_SampleType()
					{
						return SAMPLE_TYPE;
					}

					virtual ISampleContainer& get_SampleContainer()
					{
						return *m_pContainer;
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
					virtual float ReadSample(TSample*& pSource) = 0;

					int m_iAsioChannel;
					TSample* m_pBufferA;
					TSample* m_pBufferB;

					IOutputChannelPair* m_pMonitor;

					// was derived from...
					ISampleContainer* m_pContainer;
					ISampleSharer* m_pSharer;

					unsigned long m_refCount;
				};
			}
		}
	}
}



