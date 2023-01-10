using System;
using System.Globalization;
using System.Windows.Data;

namespace Lupus.Converters
{
	public class DbFSLimiter : IValueConverter
	{
		public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
		{
			if (targetType == typeof(double) || targetType == typeof(double?) || targetType == typeof(object))
			{
				// TODO: Floor and Peek are dependency properties of LevelMeter. How can we get their actual value here?
				double minValue = Lupus.Controls.LevelMeter.DefaultFloor;
				double maxValue = Lupus.Controls.LevelMeter.DefaultPeek;

				if (value is double dbFS)
				{
					return Math.Min(maxValue, Math.Max(minValue, dbFS));
				}
				return null;
			}
			throw new NotSupportedException($"Target type {targetType} is not supported. {typeof(DbFSLimiter)} supports {typeof(double)} or {typeof(double?)}.");
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
