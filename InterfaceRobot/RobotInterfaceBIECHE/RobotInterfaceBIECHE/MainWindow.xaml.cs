using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace RobotInterfaceBIECHE
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        bool isRoyalBlue = false;


        private void SendMessage()
        {
            string toSend = textBoxEmission.Text;

            bool isEmpty = false;

            if (toSend == "")
            {
                isEmpty = true;
            }

            if (!isEmpty)
            {
                textBoxReception.Text += "Reçu : " + toSend + "\n";

                textBoxEmission.Clear();
            }
        }

        private void ChangeButtonColor()
        {
            if (isRoyalBlue)
            {
                buttonEnvoyer.Background = Brushes.Beige;
                isRoyalBlue = false;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
                isRoyalBlue = true;
            }
        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e) // Cliquer plusieurs fois ne fait rien, le bouton ne reviens pas à la couleur de base par exemple.
        {
            ChangeButtonColor();
            SendMessage();
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }
    }
}