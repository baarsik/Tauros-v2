using System.ComponentModel;
using System.Runtime.CompilerServices;
using Launcher.Annotations;
using Launcher.Models;

namespace Launcher.ViewModels
{
    public class MainWindowViewModel : INotifyPropertyChanged
    {
        private MainWindowModel _model;
        private string _status;
        private string _title;

        public MainWindowModel Model
        {
            get => _model;
            set
            {
                _model = value;
                _model.PropertyChanged += (sender, args) => UpdateViewModel();
                UpdateViewModel();
            }
        }

        public string Title
        {
            get => _title;
            protected set
            {
                if (value == _title) return;
                _title = value;
                OnPropertyChanged();
            }
        }

        public string Status
        {
            get => _status;
            protected set
            {
                if (value == _status) return;
                _status = value;
                OnPropertyChanged();
            }
        }

        private void UpdateViewModel()
        {
            Title = $"Cerberus.Tauros C# Injector ({Model.LastModificationDate.ToShortDateString()})";
            Status = $"{Model.DllName} is {(Model.IsDllFound ? "found" : "not found")}";
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}