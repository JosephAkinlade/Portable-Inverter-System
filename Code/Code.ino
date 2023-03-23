#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>
#include "hmi.h"
#include "hc05.h"

namespace Pin
{
  const uint8_t pzemTx = 8;
  const uint8_t pzemRx = 9;
  const uint8_t hc05Tx = 2;
  const uint8_t hc05Rx = 3;
  const uint8_t relaySigPin = 5;
};

//Objects
SoftwareSerial pzemSerial(Pin::pzemTx,Pin::pzemRx);
SoftwareSerial hc05Serial(Pin::hc05Tx,Pin::hc05Rx);
LiquidCrystal_I2C lcd(0x27,20,4);
PZEM004Tv30 pzem(pzemSerial);
HC05 hc05(&hc05Serial);
HMI hmi(lcd);

param_t param = {0};

void setup()
{
  Serial.begin(9600);
  pinMode(Pin::relaySigPin,OUTPUT);
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(6,0);
  lcd.print("PORTABLE");
  lcd.setCursor(6,1);
  lcd.print("INVERTER");
  lcd.setCursor(7,2);
  lcd.print("SYSTEM");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("--LOADING...");
  delay(2000);
  hmi.Display_Page1(param);
}


void loop()
{
  //Get power parameters and display on LCD
//  param.volt = pzem.voltage();
//  param.curr = pzem.current();
//  param.pwr = pzem.power();
//  param.kwh = pzem.energy();
  hmi.Display_Control(param);
  //Receive units from mobile App
  if(hc05.ReceivedData())
  {
    Serial.println(hc05.DecodeData());
    hc05.ResetRxBuffer();
  }
  if(param.units > 0)
  {
    digitalWrite(Pin::relaySigPin,HIGH);   
  }
  else
  {
    digitalWrite(Pin::relaySigPin,LOW);
  }
}
