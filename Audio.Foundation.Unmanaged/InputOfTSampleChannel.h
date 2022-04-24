#pragma once
#include "pch.h"
#include "OutputOfTSampleChannelPair.h"
#include "SampleContainer.h"
#include "SampleSharer.h"
#include "IInputChannel.h"

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
				class InputOfTSampleChannel : public Audio::Foundation::SampleContainer, public Audio::Foundation::SampleSharer, public IInputChannel
				{
				public:
					/*! \brief Constructor

					*/
					InputOfTSampleChannel(int iAsioChannel, TSample* pBufferA, TSample* pBufferB, int sampleCount) :
						Audio::Foundation::SampleContainer(sampleCount),
						Audio::Foundation::SampleSharer(),
						m_iAsioChannel(iAsioChannel),
						m_pBufferA(pBufferA),
						m_pBufferB(pBufferB),
						m_pMonitor(NULL)
					{
						if (NULL == m_pBufferA || NULL == m_pBufferB)
							throw gcnew AsioCoreException("InputChannel: Buffer pointers must not be NULL.", E_INVALIDARG);
					}

					/*! \brief Destructor
					*/
					virtual ~InputOfTSampleChannel()
					{
					}

					virtual void Swap(bool readSecondHalf)
					{
						if (IsActive)
						{
							TSample* pSource = readSecondHalf ? m_pBufferB : m_pBufferA;

							// if monitor is attached, send to both monitor channels
							if (NULL != m_pMonitor)
							{
								m_pMonitor->DirectOut(pSource, true, true);
							}
							float* pDestLeft = LeftChannel->SamplePtr;
							float* pDestRight = RightChannel->SamplePtr;

							// double the voulume to get full level on both channels
							float volPanFactorLeft = 0.0f;
							float volPanFactorRight = 0.0f;
							Audio::Foundation::SampleConversion::VolumeAndPanFactor(2.0f, 0.0f, volPanFactorLeft, volPanFactorRight);

							for (int i = 0; i < SampleCount; i++)
							{
								float floatSample = ReadSample(pSource);

								*pDestLeft++ = floatSample * volPanFactorLeft;
								*pDestRight++ = floatSample * volPanFactorRight;
							}
						}
					}

					virtual void Send()
					{
						RouteToSends();
					}

					virtual IOutputChannelPair* get_Monitor()
					{
						return m_pMonitor;
					}

					virtual void put_Monitor(IOutputChannelPair* value)
					{
						m_pMonitor = value;
					}

					virtual int get_SampleType()
					{
						return SAMPLE_TYPE;
					}

					virtual ISampleContainer& get_AsSampleContainer()
					{
						return *dynamic_cast<SampleContainer*>(this);
					}

					virtual ISampleSharer& get_AsSampleSharer()
					{
						return *dynamic_cast<SampleSharer*>(this);
					}

				private:
					virtual float ReadSample(TSample*& pSource) = 0;

					int m_iAsioChannel;
					TSample* m_pBufferA;
					TSample* m_pBufferB;

					IOutputChannelPair* m_pMonitor;
				};
			}
		}
	}
}



