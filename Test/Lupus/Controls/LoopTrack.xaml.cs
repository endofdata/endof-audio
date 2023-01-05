using Audio.Foundation.Abstractions;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;

namespace Lupus.Controls
{
	/// <summary>
	/// Interaction logic for LoopTrack.xaml
	/// </summary>
	public partial class LoopTrack : UserControl
	{
		public static readonly DependencyProperty InputsProperty = DependencyProperty.Register(nameof(Inputs), typeof(IEnumerable<IAudioInput>), typeof(LoopTrack), new PropertyMetadata { DefaultValue = Array.Empty<IAudioInput>() });
		public static readonly DependencyProperty SelectedInputProperty = DependencyProperty.Register(nameof(SelectedInput), typeof(IAudioInput), typeof(LoopTrack));
		public static readonly DependencyProperty DbFSProperty = DependencyProperty.Register(nameof(DbFS), typeof(Level), typeof(LoopTrack), new PropertyMetadata { DefaultValue = LevelMeter.DbFSNegInf });
		public static readonly DependencyProperty TrackNameProperty = DependencyProperty.Register(nameof(TrackName), typeof(string), typeof(LoopTrack));
		public static readonly DependencyProperty IsHotProperty = DependencyProperty.Register(nameof(IsHot), typeof(bool), typeof(LoopTrack));
		public static readonly DependencyProperty IsMutedProperty = DependencyProperty.Register(nameof(IsMuted), typeof(bool), typeof(LoopTrack));
		public static readonly DependencyProperty IsSoloProperty = DependencyProperty.Register(nameof(IsSolo), typeof(bool), typeof(LoopTrack));
		public static readonly DependencyProperty LevelProperty = DependencyProperty.Register(nameof(Level), typeof(float), typeof(LoopTrack), new PropertyMetadata { DefaultValue = 1.0f });
		public static readonly DependencyProperty MaxLevelProperty = DependencyProperty.Register(nameof(MaxLevel), typeof(float), typeof(LoopTrack), new PropertyMetadata { DefaultValue = DefaultMaxLevel });
		public static readonly DependencyProperty PanProperty = DependencyProperty.Register(nameof(Pan), typeof(float), typeof(LoopTrack));

		public const float DefaultMaxLevel = 1.2f;

		public IEnumerable<IAudioInput> Inputs
		{
			get => (IEnumerable<IAudioInput>)GetValue(InputsProperty);
			set => SetValue(InputsProperty, value);
		}

		public IAudioInput SelectedInput
		{
			get => (IAudioInput)GetValue(SelectedInputProperty);
			set => SetValue(SelectedInputProperty, value);
		}

		public string? TrackName
		{
			get => (string?)GetValue(TrackNameProperty);
			set => SetValue(TrackNameProperty, value);
		}

		public bool IsHot
		{
			get => (bool)GetValue(IsHotProperty);
			set => SetValue(IsHotProperty, value);
		}

		public bool IsMuted
		{
			get => (bool)GetValue(IsMutedProperty);
			set => SetValue(IsMutedProperty, value);
		}

		public bool IsSolo
		{
			get => (bool)GetValue(IsSoloProperty);
			set => SetValue(IsSoloProperty, value);
		}

		public float Level
		{
			get => (float)GetValue(LevelProperty);
			set => SetValue(LevelProperty, value);
		}
		public float MaxLevel
		{
			get => (float)GetValue(MaxLevelProperty);
			set => SetValue(MaxLevelProperty, value);
		}

		public float Pan
		{
			get => (float)GetValue(PanProperty);
			set => SetValue(PanProperty, value);
		}

		public Level DbFS
		{
			get => (Level)GetValue(DbFSProperty);
			set => SetValue(DbFSProperty, value);
		}
		
		public LoopTrack()
		{
			InitializeComponent();
		}
	}
}
