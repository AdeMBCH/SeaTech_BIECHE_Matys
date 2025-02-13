#ifndef UART_PROTOCOL
#define UART_PROTOCOL


unsigned char UartCalculateChecksum(int,int, unsigned char*);
void UartEncodeAndSendMessage(int ,int, unsigned char*);
void UartDecodeMessage(unsigned char);
void UartProcessDecodedMessage(int ,int , unsigned char* );



void SendLedMessage(unsigned char, unsigned char );
void SendIrDistanceMessage(float , float , float );
void SendMotorSpeedMessage(float , float );

#endif /* UART_PROTOCOL */

