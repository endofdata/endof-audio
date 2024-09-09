#include "pch.h"
#include "ControllerMapping.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

ControllerMapping::ControllerMapping(ControllerCode code, int commandId)
{
	m_commandId = commandId;
	m_code = code;
}

ControllerCode ControllerMapping::get_Code() const
{
	return m_code;
}

int ControllerMapping::get_CommandId() const
{
	return m_commandId;
}
