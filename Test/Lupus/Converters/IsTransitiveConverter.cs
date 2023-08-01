using Audio.Asio.Interop;
using System;
using System.Globalization;
using System.Windows.Data;

namespace Lupus.Converters
{
	class IsTransitiveConverter : IValueConverter
	{
		public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
		{
			if (value is RecordingMode recordingMode)
			{
				return recordingMode switch
				{
					RecordingMode.Armed or RecordingMode.Unarmed => true,
					_ => false
				};
			}
			return null;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
