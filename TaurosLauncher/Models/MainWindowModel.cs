using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Launcher.Annotations;

namespace Launcher.Models
{
    public sealed class MainWindowModel : INotifyPropertyChanged
    {
        private bool _isDllFound;
        public string DllName { get; set; }
        public DateTime LastModificationDate { get; set; }

        public bool IsDllFound
        {
            get => _isDllFound;
            set
            {
                if (value == _isDllFound) return;
                _isDllFound = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}