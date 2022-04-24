#pragma once

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
#pragma message("defining struct Level")
			public value struct Level sealed
			{
			public:
				Level(double left, double right)
				{
					_left = left;
					_right = right;
				}

				property double Left
				{
					double get()
					{
						return _left;
					}
				}

				property double Right
				{
					double get()
					{
						return _right;
					}
				}

			private:
				double _left;
				double _right;
			};
		}
	}
}
