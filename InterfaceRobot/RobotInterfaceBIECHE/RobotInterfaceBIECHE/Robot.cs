using System;

public class Robot
{
    public string receivedText = "";
    public float distanceTelemetreDroit;
    public float distanceTelemetreCentre;
    public float distanceTelemetreGauche;
    public Queue<byte> byteListReceived = new Queue<byte>();
    public Robot()
    {
        
    }
}
