using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Lupus.Model
{
	internal class NotifyPropertyChangedBase : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler? PropertyChanged;

		protected virtual bool SetValue<T>(ref T backingField, T value, [CallerMemberName] string? propertyName = null)
		{
			var isModified = !object.Equals(backingField, value);

			if (isModified)
			{
				backingField = value;
				OnPropertyChange(propertyName);
			}
			return isModified;
		}

		protected virtual void OnPropertyChange([CallerMemberName] string? propertyName = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
	}
}
