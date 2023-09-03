#pragma once

#include "Audio.Osc.Unamanged.h"
#include "TypeTag.h"
#include "PacketBase.h"
#include "OscString.h"

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
				static std::shared_ptr<MessageBuilder> Create(std::istream& istr);

				MessageBuilder();

				const char* get_AddressPattern() const;
				void put_AddressPattern(const char* address);
				_declspec(property(get = get_AddressPattern, put = put_AddressPattern)) const char* AddressPattern;

				void AllocParameters(const TypeTag tags[], int count);
				void AllocParameters(const TypeTag tags[], int count, int varSize[], int varSizeCount);
				void SetNextParameter(const TypeTag tag, const void* value);
				void SetAllParameters(std::istream& istr);

				void GetNextParameter(TypeTag& tag, const void*& value, int& size);

				int get_ParameterCount() const;
				_declspec(property(get = get_ParameterCount)) int ParameterCount;

				const TypeTag  get_ParameterType(int idx) const;
				_declspec(property(get = get_ParameterType)) TypeTag ParameterType[];

				virtual int get_Size() const;
				_declspec(property(get = get_Size)) int Size;

				std::ostream& Write(std::ostream& ostr) const;

				static int GetParameterSize(const TypeTag tag);
				static bool IsVariantSize(const TypeTag tag);

				friend _AUDIO_OSC_UNMANAGED_API std::ostream& operator << (std::ostream& ostr, const MessageBuilder& it);

			private:

				OscString m_addressPattern;
				std::vector<TypeTag> m_parameterTypes;
				std::vector<int> m_variantSizes;
				std::unique_ptr<char[]> m_parameters;

				int m_parametersSize;
				char* m_parametersWritePos;
				int m_parametersWritten;
				int m_variantSizesWritten;
			};			
		}
	}
}