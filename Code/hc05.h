#pragma once

#include <SoftwareSerial.h>

#define RX_BUFFER_SIZE  4


class HC05
{
  private:
    SoftwareSerial* port;
    int ConvStrToInt(char* str,int len);
    char RxBuffer[RX_BUFFER_SIZE];

  public:
    HC05(SoftwareSerial* serial, uint32_t baudRate = 9600);
    bool ReceivedData(void); 
    int DecodeData(void);
    void ResetRxBuffer(void);
};
