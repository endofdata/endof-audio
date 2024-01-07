using System.Windows.Input;

namespace Lupus
{
	internal static class CustomCommands
	{
		public static readonly RoutedUICommand Record = new("REC", nameof(Record), typeof(MainWindow));
		public static readonly RoutedUICommand Run = new("RUN", nameof(Run), typeof(MainWindow));
		public static readonly RoutedUICommand Rewind = new("REW", nameof(Rewind), typeof(MainWindow));
		public static readonly RoutedUICommand SoloTrack = new("SOLO", nameof(SoloTrack), typeof(MainWindow));
		public static readonly RoutedUICommand MuteTrack = new("MUTE", nameof(MuteTrack), typeof(MainWindow));
		public static readonly RoutedUICommand DeleteTrack = new("DELETE", nameof(DeleteTrack), typeof(MainWindow));
		public static readonly RoutedUICommand Configure = new("CONFIGURE", nameof(Configure), typeof(MainWindow));
	}
}
