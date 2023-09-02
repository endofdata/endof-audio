#pragma once

#include "Audio.Osc.Unamanged.h"
#include "TypeTag.h"
#include "PacketBase.h"

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace Audio
{
	namespace Osc
	{
		namespace Unmanaged
		{
			class _AUDIO_OSC_UNMANAGED_API MessageBuilder : public PacketBase
			{
			public:
				MessageBuilder();

				void put_AddressPattern(const char* address);
				const char* get_AddressPattern() const;

				void AllocParameters(const TypeTag tags[], int count);
				void AllocParameters(const TypeTag tags[], int count, int varSize[], int varSizeCount);
				void WriteParameter(const TypeTag tag, const void* value);

				int get_Size() const;

				std::ostream& Write(std::ostream& ostr) const;

				friend std::ostream& operator << (std::ostream& ostr, const MessageBuilder& it);
				
			private:
				int GetParameterSize(const TypeTag tag);
				bool IsVariantSize(const TypeTag tag);

				std::string m_addressPattern;
				std::vector<TypeTag> m_parameterTypes;
				std::vector<int> m_variantSizes;

				char* m_parameters;
				int m_parametersSize;
				char* m_parametersWritePos;
				int m_parametersWritten;
				int m_variantSizesWritten;
			};			
		}
	}
}