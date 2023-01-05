using Audio.Foundation.Abstractions;
using System.Windows;
using System.Windows.Controls;

namespace Lupus.Controls
{
	/// <summary>
	/// Interaction logic for LevelMeter.xaml
	/// </summary>
	public partial class LevelMeter : UserControl
	{
		public const double DefaultFloor = -40.0;
		public const double DefaultPeek = 2.0;
		public static readonly Level DbFSNegInf = new(double.NegativeInfinity, double.NegativeInfinity);

		public readonly static DependencyProperty FloorProperty = DependencyProperty.Register(nameof(Floor), typeof(double), typeof(LevelMeter), new PropertyMetadata { DefaultValue = DefaultFloor });
		public readonly static DependencyProperty PeekProperty = DependencyProperty.Register(nameof(Peek), typeof(double), typeof(LevelMeter), new PropertyMetadata { DefaultValue = DefaultPeek });
		public readonly static DependencyProperty ValueProperty = DependencyProperty.Register(nameof(Value), typeof(double), typeof(LevelMeter), new PropertyMetadata { DefaultValue = DefaultFloor });

		public double Floor
		{
			get => (double)GetValue(FloorProperty);
			set => SetValue(FloorProperty, value);
		}

		public double Peek
		{
			get => (double)GetValue(PeekProperty);
			set => SetValue(PeekProperty, value);
		}
		public double Value
		{
			get => (double)GetValue(ValueProperty);
			set => SetValue(ValueProperty, value);
		}

		public LevelMeter()
		{
			InitializeComponent();
		}
	}
}
