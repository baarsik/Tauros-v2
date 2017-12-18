using System;
using System.Windows;

namespace Launcher
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Inject_Click(object sender, RoutedEventArgs e)
        {
            var result = Injector.Inject("csgo", $"{AppDomain.CurrentDomain.BaseDirectory}Tauros.dll");
            if (!result)
            {
                MessageBox.Show("Injection failed", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            Application.Current.Shutdown();
        }
    }
}
