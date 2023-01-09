#pragma once

#include "ISampleBuffer.h"
#include <vector>

#define DECLARE_SAMPLECONTAINER												\
	virtual bool get_IsActive();											\
	virtual void put_IsActive(bool value);									\
	virtual int get_SampleCount();											\
	virtual void put_SampleCount(int sampleCount);							\
	virtual int get_ChannelCount();											\
	virtual void put_ChannelCount(int channelCount);						\
	virtual Abstractions::ISampleBuffer* get_Channel(int index);

#define IMPLEMENT_SAMPLECONTAINER(TYPE_NAME)								\
																			\
	bool TYPE_NAME::get_IsActive()											\
	{																		\
		return SampleContainerBase::get_IsActive();							\
	}																		\
																			\
	void TYPE_NAME::put_IsActive(bool value)								\
	{																		\
		SampleContainerBase::put_IsActive(value);							\
	}																		\
																			\
	int TYPE_NAME::get_SampleCount()										\
	{																		\
		return SampleContainerBase::get_SampleCount();						\
	}																		\
																			\
	void TYPE_NAME::put_SampleCount(int sampleCount)						\
	{																		\
		return SampleContainerBase::put_SampleCount(sampleCount);			\
	}																		\
																			\
	int TYPE_NAME::get_ChannelCount()										\
	{																		\
		return SampleContainerBase::get_ChannelCount();						\
	}																		\
																			\
	void TYPE_NAME::put_ChannelCount(int channelCount)						\
	{																		\
		return SampleContainerBase::put_ChannelCount(channelCount);			\
	}																		\
																			\
	Abstractions::ISampleBuffer* TYPE_NAME::get_Channel(int index)			\
	{																		\
		return SampleContainerBase::get_Channel(index);						\
	}

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleContainerBase
			{
			public:
				SampleContainerBase(int sampleCount, int channelCount);
				virtual ~SampleContainerBase();

				virtual bool get_IsActive();
				virtual void put_IsActive(bool value);

				virtual int get_SampleCount();
				virtual void put_SampleCount(int sampleCount);

				virtual int get_ChannelCount();
				virtual void put_ChannelCount(int channelCount);

				virtual Abstractions::ISampleBuffer* get_Channel(int index);

			private:
				void CreateChannels(int sampleCount, int channelCount);
				void FreeChannels();

				std::vector<Abstractions::ISampleBuffer*> m_vecChannels;
				int m_sampleCount;
				bool m_isActive;
			};
		}
	}
}
