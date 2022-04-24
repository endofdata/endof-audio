#pragma once

namespace Audio
{
	namespace Foundation
	{
		public ref class ExceptionExtensions sealed
		{
		public:
			static System::String^ GetMessageChain(System::Exception^ ex);

		private:
			static System::Collections::Generic::IEnumerable<System::String^>^ RecurseMessages(System::Exception^ ex);
		};
	}
}