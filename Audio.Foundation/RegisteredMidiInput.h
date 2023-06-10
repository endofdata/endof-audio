#pragma once

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class RegisteredMidiInput sealed
			{
			public:
				property unsigned int Id
				{
					unsigned int get();
				}

				property System::String^ Name
				{
					System::String^ get();
				}

				virtual System::String^ ToString() override;

			internal:
				RegisteredMidiInput(unsigned int id, System::String^ name);


			private:
				unsigned int _id;
				System::String^ _name;
			};
		}
	}
}