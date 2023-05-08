#pragma once
#include "pch.h"
#include "SampleContainer.h"
#include "IInputChannel.h"
#include "SampleConversionUnmanaged.h"

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
				class InputOfTSampleChannel : public IInputChannel
				{
				public:
					/*! \brief Constructor

					*/
					InputOfTSampleChannel(int iAsioChannel, TSample* pBufferA, TSample* pBufferB, int sampleCount) :
						m_asioChannelId(iAsioChannel),
						m_pBufferA(pBufferA),
						m_pBufferB(pBufferB),
						m_pDirectMonitor(nullptr),
						m_isActive(false),
						m_refCount(0)
					{
						if (nullptr == m_pBufferA || nullptr == m_pBufferB)
							throw std::invalid_argument("InputChannel: Buffer pointers must not be nullptr.");
					}

					/*! \brief Destructor
					*/
					virtual ~InputOfTSampleChannel()
					{
					}

					virtual void OnNextBuffer(ISampleContainerPtr& container, bool readSecondHalf)
					{
						if (IsActive)
						{
							TSample* pSource;
							SelectBufferPointer(readSecondHalf, pSource);

							// if monitor is attached, send to both monitor channels
							if (nullptr != m_pDirectMonitor)
							{
								m_pDirectMonitor->DirectOut(!readSecondHalf, pSource, true, true);
							}

							// Convert the incoming samples to internal and write them to the container
							int sampleCount = container->SampleCount;

							if (container->ChannelCount > 1)
							{
								double factorLeft = m_mix.FactorLeft;
								double factorRight = m_mix.FactorRight;
								Sample* pDestLeft = container->Channels[0]->SamplePtr;
								Sample* pDestRight = container->Channels[1]->SamplePtr;
								
								for (int i = 0; i < sampleCount; i++)
								{
									Sample sample = ReadSample(pSource);

									*(pDestLeft++) += static_cast<Sample>(factorLeft * sample);
									*(pDestRight++) += static_cast<Sample>(factorRight * sample);
								}
							}
							else if (container->ChannelCount > 0)
							{
								double level = m_mix.Level;
								Sample* pDest = container->Channels[0]->SamplePtr;

								for (int i = 0; i < sampleCount; i++)
								{
									*pDest++ += static_cast<Sample>(level * ReadSample(pSource));
								}
							}
						}
					}

					virtual bool get_IsActive()
					{
						return m_isActive;
					}

					virtual void put_IsActive(bool value)
					{
						m_isActive = value;
					}

					virtual MixParameter& get_Mix()
					{
						return m_mix;
					}

					virtual bool get_SupportsDirectMonitor()
					{
						return true;
					}

					virtual IOutputChannelPairPtr& get_DirectMonitor()
					{
						return m_pDirectMonitor;
					}

					virtual void put_DirectMonitor(IOutputChannelPairPtr& value)
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

					virtual int get_Id()
					{
						return m_asioChannelId;
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
						*pResult = nullptr;
						return false;
					}

				private:
					void SelectBufferPointer(bool readSecondHalf, TSample*& pSource)
					{
						pSource = readSecondHalf ? m_pBufferB : m_pBufferA;
					}

					virtual Sample ReadSample(TSample*& pSource) = 0;

					int m_asioChannelId;
					TSample* m_pBufferA;
					TSample* m_pBufferB;

					bool m_isActive;
					MixParameter m_mix;
					IOutputChannelPairPtr m_pDirectMonitor;

					unsigned long m_refCount;
				};
			}
		}
	}
}



