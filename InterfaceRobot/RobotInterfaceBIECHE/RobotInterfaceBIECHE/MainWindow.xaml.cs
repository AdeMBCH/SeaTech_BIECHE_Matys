using System;
using System.IO.Ports;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;
using ExtendedSerialPort_NS;

namespace RobotInterfaceBIECHE
{
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();

        public MainWindow()
        {
            InitializeComponent();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            serialPort1 = new ExtendedSerialPort("COM5", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
        }

        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            var byteQueue = robot.byteListReceived;
            while (byteQueue.Count > 0)
            {
                byte b = byteQueue.Dequeue();
                textBoxReception.Text += $"0x{b.ToString("X2")} ";  // Format 0xhh
                DecodeMessage(b);
            }

            robot.receivedText = string.Empty;
        }

        private void SerialPort1_DataReceived(object? sender, DataReceivedArgs e)
        {
            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
        }

        private void SendMessage()
        {
            byte[] msgPayload = Encoding.ASCII.GetBytes(textBoxEmission.Text);
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = (int)CommandId.Text;
            UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);

            textBoxEmission.Clear();
        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
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
            SendTextMessage("Bonjour");
            SendLedMessage(1, 1); // LED 1 Allumée
            SendIrDistanceMessage(10, 20, 30);
            SendMotorSpeedMessage(50, 75);
        }

        private void SendTextMessage(string text)
        {
            byte[] msgPayload = Encoding.ASCII.GetBytes(text);
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = (int)CommandId.Text;
            ProcessDecodedMessage(msgFunction, msgPayloadLength, msgPayload);
        }

        private void SendLedMessage(byte ledNumber, byte ledState)
        {
            byte[] msgPayload = new byte[] { ledNumber, ledState };
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = (int)CommandId.Led;
            ProcessDecodedMessage(msgFunction, msgPayloadLength, msgPayload);
        }

        private void SendIrDistanceMessage(byte leftDistance, byte centerDistance, byte rightDistance)
        {
            byte[] msgPayload = new byte[] { leftDistance, centerDistance, rightDistance };
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = (int)CommandId.IrDistance;
            ProcessDecodedMessage(msgFunction, msgPayloadLength, msgPayload);
        }

        private void SendMotorSpeedMessage(byte leftSpeed, byte rightSpeed)
        {
            byte[] msgPayload = new byte[] { leftSpeed, rightSpeed };
            int msgPayloadLength = msgPayload.Length;
            int msgFunction = (int)CommandId.MoteurV;
            ProcessDecodedMessage(msgFunction, msgPayloadLength, msgPayload);
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
        }

        public enum CommandId
        {
            Text = 0x0080,
            Led = 0x0020,
            IrDistance = 0x0030,
            MoteurV = 0x0040
        }

        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch (msgFunction)
            {
                case (int)CommandId.Text:
                    string receivedText = Encoding.ASCII.GetString(msgPayload);
                    textBoxReception.Text = receivedText;
                    break;

                case (int)CommandId.Led:
                    if (msgPayloadLength == 2)
                    {
                        byte ledNumber = msgPayload[0];
                        byte ledState = msgPayload[1];

                        if (ledNumber == 1)
                        {
                            checkboxLed1.IsChecked = (ledState == 1);
                        }
                        else if (ledNumber == 2)
                        {
                            checkboxLed2.IsChecked = (ledState == 1);
                        }
                        else if (ledNumber == 3)
                        {
                            checkboxLed3.IsChecked = (ledState == 1);
                        }
                    }
                    break;

                case (int)CommandId.IrDistance:
                    if (msgPayloadLength == 3)
                    {
                        byte leftDistance = msgPayload[0];
                        byte centerDistance = msgPayload[1];
                        byte rightDistance = msgPayload[2];

                        textBlockIrGauche.Text = "IR Gauche : " + leftDistance.ToString();
                        textBlockIrCentre.Text = "IR Centre : " + centerDistance.ToString();
                        textBlockIrDroit.Text = "IR Droit : " + rightDistance.ToString();
                    }
                    break;

                case (int)CommandId.MoteurV:
                    if (msgPayloadLength == 2)
                    {
                        byte leftSpeed = msgPayload[0];
                        byte rightSpeed = msgPayload[1];

                        textBlockMoteurGauche.Text = "Vitesse Gauche : " + leftSpeed.ToString();
                        textBlockMoteurDroit.Text = "Vitesse Droit : " + rightSpeed.ToString();
                    }
                    break;

                default:
                    break;
            }
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

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = (c << 8) + msgDecodedFunction;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction = (c << 0) + msgDecodedFunction;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = (c << 8) + msgDecodedPayloadLength;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength = (c << 0) + msgDecodedPayloadLength;
                    if (msgDecodedPayloadLength > 1024)
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
                    byte receivedChecksum = c;
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

                    if (receivedChecksum == calculatedChecksum)
                    {
                        textBoxReception.Text += "\n";
                        textBoxReception.Text += "Message valide \n";
                        textBoxReception.Text += "Fonction : " + msgDecodedFunction + "\n";
                        textBoxReception.Text += "Taille Payload: " + msgDecodedPayloadLength + "\n";
                        textBoxReception.Text += "Payload : " + Encoding.ASCII.GetString(msgDecodedPayload) + "\n";

                        UartEncodeAndSendMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload); 
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
