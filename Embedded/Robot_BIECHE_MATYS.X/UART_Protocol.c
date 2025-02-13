#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"


unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction prenant entree la trame et sa longueur pour calculer le checksum
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (unsigned char)(msgFunction >> 8);
    checksum ^= (unsigned char)(msgFunction >> 0); // Par symétrie de construction
    checksum ^= (unsigned char)(msgPayloadLength >> 8);
    checksum ^= (unsigned char)(msgPayloadLength >> 0);

    for (int i = 0; i < msgPayloadLength; i++)
    {
        checksum ^= msgPayload[i];
    }

    return checksum;
}


void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char *msgPayload)
{
    unsigned char message[6 + msgPayloadLength];
    int pos = 0;
    message[pos++] = 0xFE;
    message[pos++] = (unsigned char)(msgFunction >> 8);
    message[pos++] = (unsigned char)(msgFunction >> 0);
    message[pos++] = (unsigned char)(msgPayloadLength >> 8);
    message[pos++] = (unsigned char)(msgPayloadLength >> 0);
    for (int i = 0; i < msgPayloadLength; i++)
    {
        message[pos++] = msgPayload[i];
    }
    unsigned char checksum = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    message[pos++] = checksum;
    SendMessage(message, pos);
}


int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;

enum StateReception {
    Waiting,
    FunctionMSB,
    FunctionLSB,
    PayloadLengthMSB,
    PayloadLengthLSB,
    Payload,
    CheckSum
} rcvState;

void UartDecodeMessage(unsigned char c) {
}

void UartProcessDecodedMessage(int function,int payloadLength, unsigned char* payload)
{
//Fonction appelee apres le decodage pour executer l?action
//correspondant au message recu

}

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/

int Text = 0x0080;
int Led = 0x0020;
int IrDistance = 0x0030;
int MoteurV = 0x0040;

    void SendLedMessage(unsigned char ledNumber, unsigned char ledState)
    {
        unsigned char msgPayload[] = { ledNumber, ledState };
        int msgPayloadLength = sizeof(msgPayload) / sizeof(msgPayload[0]);
        int msgFunction = Led;
        UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
    }

    void SendIrDistanceMessage(float leftDistance, float centerDistance, float rightDistance)
    {
        unsigned char msgPayload[] = { leftDistance, centerDistance, rightDistance };
        int msgPayloadLength = sizeof(msgPayload) / sizeof(msgPayload[0]);
        int msgFunction = IrDistance;
        UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
    }

    void SendMotorSpeedMessage(float leftSpeed, float rightSpeed)
    {
        unsigned char msgPayload[] = { leftSpeed, rightSpeed };
        int msgPayloadLength = sizeof(msgPayload) / sizeof(msgPayload[0]);
        int msgFunction = MoteurV;
        UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
    }
    
    