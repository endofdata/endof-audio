#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>
#include <ITake.h>
#include <UnknownBase.h>
#include <AudioTime.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class Take : public ITake
			{
			public:
				Take(int id, ISampleContainerPtr& container, AudioTime position, AudioTime length);
				virtual ~Take();

				virtual int get_Id() const;

				virtual AudioTime get_Position() const;
				virtual void put_Position(AudioTime value);

				virtual AudioTime get_Length() const;
				virtual void put_Length(AudioTime value);

				virtual AudioTime get_EndPosition() const;

				virtual ISampleContainerPtr get_Container();

				virtual bool HasDataAt(AudioTime position) const;

				bool SeekTo(AudioTime offset, AudioSeek kind);

				int WriteTo(ISampleContainerPtr& target, int sampleOffset, int sampleCount, int channelOffset, int channelCount, 
					int targetSampleOffset, int targetChannelOffset, const MixParameter& mix, bool overdub) const;

				int WriteTo(ISampleContainerPtr& target, int channelOffset, int channelCount, int targetChannelOffset);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				int m_id;
				ISampleContainerPtr m_container;
				AudioTime m_position;
				AudioTime m_length;
				AudioTime m_readOffset;
			};
			
		}
	}
}
