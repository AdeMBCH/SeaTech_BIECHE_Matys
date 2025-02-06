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
using System;


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

            serialPort1 = new ExtendedSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
            InitializeComponent();

        }

        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            //textBoxReception.Text += robot.receivedText;
            //robot.receivedText = string.Empty;
            var byteQueue = robot.byteListReceived;
            while (byteQueue.Count > 0)
            {
                byte b = byteQueue.Dequeue();
                textBoxReception.Text += $"0x{b.ToString("X2")} " ;  // Format 0xhh
                DecodeMessage(b);
            }

            robot.receivedText = string.Empty;
        }
        private void SerialPort1_DataReceived(object? sender, DataReceivedArgs e)
        {
            //textBoxReception.Text += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
        }

        bool isRoyalBlue = false;

        private void SendMessage()
        {

            //serialPort1.WriteLine(textBoxEmission.Text);

            //textBoxReception.Text += "Reçu : " + toSend + "\n";

            byte[] msgPayload = Encoding.ASCII.GetBytes(textBoxEmission.Text);
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = 0x0080;
            UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);

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

            //serialPort1.Write(byteList, 0, byteList.Length);
            string messageStr = "Bonjour";
            byte[] msgPayload = Encoding.ASCII.GetBytes(messageStr);
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = 0x0080;
            UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
        }

        private byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;
            checksum ^= 0xFE;
            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte)(msgFunction >> 0); // On le fait par symétrie de construction
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength >> 0);

            foreach (byte bt in msgPayload)
            {
                checksum ^= bt;
            }
            return checksum;
        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] message = new byte[6 + msgPayloadLength];
            int pos = 0;
            message[pos++] = 0xFE;
            message[pos++] = (byte)(msgFunction >> 8);
            message[pos++] = (byte)(msgFunction >> 0);
            message[pos++] = (byte)(msgPayloadLength >> 8);
            message[pos++] = (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                message[pos++] = msgPayload[i];
            }
            byte checksum = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
            message[pos++] = checksum;
            serialPort1.Write(message, 0, pos);
            // Attention il faudrait ajouter une protection avant et après le payloadlength.
            // En effet, imaginons quel'on veuille envoyer 650000 caractères
            // mais que finalement on veuille plus
            // il faudrait pouvoir arrêter le processus avant l'envoie sinon on va attendre l'envoie des 650000 caractères
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState =StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c== 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction = c << 0;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength = c << 0;
                    if(msgDecodedPayloadLength>1024)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    else if (msgDecodedPayloadLength > 0)
                    {
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                        rcvState = StateReception.Payload;
                    }
                    else
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;
                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;
                case StateReception.CheckSum:
                    byte receivedChecksum = c ;
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

                    if (receivedChecksum == calculatedChecksum)
                    {
                        textBoxReception.Text += "\n";
                        textBoxReception.Text += "Message valide \n";
                        textBoxReception.Text += "Fonction : " + msgDecodedFunction + "\n";
                        textBoxReception.Text += "Taille Payload: " + msgDecodedPayloadLength + "\n";
                        textBoxReception.Text += "Payload : " + Encoding.ASCII.GetString(msgDecodedPayload) + "\n";
                    }
                    else
                    {
                        textBoxReception.Text += "Erreur Checksum \n";
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }
    }
}



