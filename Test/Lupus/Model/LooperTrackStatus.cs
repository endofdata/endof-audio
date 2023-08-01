namespace Lupus.Model
{
	class LooperTrackStatus : NotifyPropertyChangedBase
    {
		private string? _name;
		private float _pan;
		private float _gain;

		public string? Name
        {
            get => _name;
            set => SetValue(ref _name, value);
        }

        public float Pan
        {
            get => _pan;
            set => SetValue(ref _pan, value);
        }

        public float Gain
        {
            get => _gain;
            set => SetValue(ref _gain, value);
        }
    }
}
