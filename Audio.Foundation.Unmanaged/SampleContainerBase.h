#pragma once

#include "ISampleBuffer.h"

#define DECLARE_SAMPLECONTAINER												\
	virtual Abstractions::ISampleBuffer* get_LeftChannel();					\
	virtual Abstractions::ISampleBuffer* get_RightChannel();				\
	virtual bool get_IsActive();											\
	virtual void put_IsActive(bool value);									\
	virtual int get_SampleCount();											\
	virtual void put_SampleCount(int sampleCount);

#define IMPLEMENT_SAMPLECONTAINER(TYPE_NAME)								\
	Abstractions::ISampleBuffer* TYPE_NAME::get_LeftChannel()				\
	{																		\
		return SampleContainerBase::get_LeftChannel();						\
	}																		\
																			\
	Abstractions::ISampleBuffer* TYPE_NAME::get_RightChannel()				\
	{																		\
		return SampleContainerBase::get_RightChannel();						\
	}																		\
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
				SampleContainerBase(int sampleCount);
				virtual ~SampleContainerBase();

				virtual Abstractions::ISampleBuffer* get_LeftChannel();

				//_declspec(property(get = get_LeftChannel)) ISampleBuffer* LeftChannel;

				virtual Abstractions::ISampleBuffer* get_RightChannel();

				//_declspec(property(get = get_RightChannel)) ISampleBuffer* RightChannel;

				virtual bool get_IsActive();
				virtual void put_IsActive(bool value);

				//_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

				virtual int get_SampleCount();
				virtual void put_SampleCount(int sampleCount);

				//_declspec(property(get = get_SampleCount, put = put_SampleCount)) int SampleCount;

			private:
				void AllocChannels(int sampleCount);
				void FreeChannels();

				Abstractions::ISampleBuffer* m_pLeftChannel;
				Abstractions::ISampleBuffer* m_pRightChannel;
				int m_sampleCount;
				bool m_isActive;
			};
		}
	}
}
