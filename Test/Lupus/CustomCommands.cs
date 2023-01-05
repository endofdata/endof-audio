using System.Windows.Input;

namespace Lupus
{
	internal static class CustomCommands
	{
		public static readonly RoutedUICommand Record = new("REC", nameof(Record), typeof(MainWindow));
		public static readonly RoutedUICommand Run = new("RUN", nameof(Run), typeof(MainWindow));
		public static readonly RoutedUICommand Rewind = new("REW", nameof(Rewind), typeof(MainWindow));
	}
}
