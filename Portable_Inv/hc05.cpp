#include <Arduino.h>
#include "hc05.h"

int HC05::ConvStrToInt(char* str,int len)
{//Max of 3-digit integers
  const int powerOfTen[] = {1,10,100}; 
  int integer = 0;
  for(int i = 0; i < len; i++)
  {
    integer += ((str[i]-'0')*powerOfTen[len-i-1]);
  }
  return integer;
}

HC05::HC05(SoftwareSerial* serial, uint32_t baudRate)
{
  //Initialize private variables
  this->port = serial;
  this->port->begin(baudRate);
  for(uint8_t i = 0; i < RX_BUFFER_SIZE; i++)
  {
    RxBuffer[i] = 0;
  }
}

bool HC05::ReceivedData(void)
{
  bool rxDone = false;
  while(port->available())
  {
    port->readBytes(RxBuffer, 5);
    rxDone = true;
  }
  return rxDone;
}

int HC05::DecodeData(void)
{
  return ConvStrToInt(RxBuffer,strlen(RxBuffer));
}

void HC05::ResetRxBuffer(void)
{
  memset(RxBuffer,0,RX_BUFFER_SIZE);
}
