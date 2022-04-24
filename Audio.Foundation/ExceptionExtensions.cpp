#include "pch.h"
#include "ExceptionExtensions.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace Audio::Foundation;

// static
String^ ExceptionExtensions::GetMessageChain(Exception^ ex)
{
	return String::Join("\r\n=> ", RecurseMessages(ex));
}

// static 
IEnumerable<System::String^>^ ExceptionExtensions::RecurseMessages(System::Exception^ ex)
{
	List<String^>^ result = gcnew List<String^>();

	while (ex != nullptr)
	{
		result->Add(ex->Message);
		ex = ex->InnerException;
	}

	return result;
}
