using System;
using System.IO;
using System.Windows;
using Launcher.Models;
using Launcher.ViewModels;

namespace Launcher
{
    public partial class MainWindow : Window
    {
        private readonly MainWindowViewModel _viewModel;

        public MainWindow()
        {
            _viewModel = new MainWindowViewModel
            {
                Model = new MainWindowModel
                {
                    DllName = "Tauros.dll",
                    LastModificationDate = new DateTime(2018, 09, 08)
                }
            };

            this.DataContext = _viewModel;

            VerifyIfFileExists();
            InitializeComponent();
        }

        private void VerifyIfFileExists()
        {
            _viewModel.Model.IsDllFound = File.Exists($"{AppDomain.CurrentDomain.BaseDirectory}{_viewModel.Model.DllName}");
        }

        private void Inject_Click(object sender, RoutedEventArgs e)
        {
            var result = Injector.Inject("csgo", $"{AppDomain.CurrentDomain.BaseDirectory}{_viewModel.Model.DllName}");
            if (!result)
            {
                VerifyIfFileExists();
                MessageBox.Show("Injection failed", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            Application.Current.Shutdown();
        }
    }
}
