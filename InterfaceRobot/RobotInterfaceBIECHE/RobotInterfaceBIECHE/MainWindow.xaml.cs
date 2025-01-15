using System.IO.Ports;
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
using ExtendedSerialPort_NS;
using System.Windows.Threading;


namespace RobotInterfaceBIECHE
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();

        public MainWindow()
        {

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            serialPort1 = new ExtendedSerialPort("COM7", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
            InitializeComponent();

        }

        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            textBoxReception.Text += robot.receivedText;
            robot.receivedText = string.Empty;
        }
        private void SerialPort1_DataReceived(object? sender, DataReceivedArgs e)
        {
            // textBoxReception.Text += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            for (int i = 0; i < 20; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
        }

        bool isRoyalBlue = false;

        private void SendMessage()
        {

            serialPort1.WriteLine(textBoxEmission.Text);

            //textBoxReception.Text += "Reçu : " + toSend + "\n";

            textBoxEmission.Clear();
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
            //ChangeButtonColor();
            SendMessage();
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Clear();
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            byte[] byteList = new byte[20];
            for (int i = 0; i < 20; i++)
            {
                byteList[i] = (byte)(2*i);
            }

            serialPort1.Write(byteList, 0, byteList.Length);

        }
    }
}



