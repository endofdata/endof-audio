using Audio.Foundation.Interop;
using System;
using System.Globalization;
using System.Windows.Data;

namespace Lupus.Converters
{
	internal class TimeStringConverter : IValueConverter
	{
		public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
		{
			if (value == null)
			{
				return null;
			}
			if (value is ManagedAudioTime time && targetType == typeof(string))
			{
				return $"{time.Minutes:D02}:{time.Seconds:D02}:{time.MilliSeconds:D03}";
			}
			return null;
		}

		public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
