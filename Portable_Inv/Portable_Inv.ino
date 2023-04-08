/*
 * This code implements a portable inverter system that uses a PZEM004Tv30 
 * power meter to measure voltage, current, power and energy consumption.
 * The system also uses a HC-05 blutooth module to receive commands from a 
 * mobile app and uses the on-board EEPROM to store available units of energy.
 * The LCD display shows power parameters and the available units of energy, while
 * a relay controls the power output based on the available units. The system 
 * automatically decrements the available units based on energy consumption and cuts off
 * supply when the available units run out
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>
#include <EEPROM.h>
#include <MapFloat.h>
#include "hmi.h"
#include "hc05.h"

#define EEPR_UNITS  1
#define EEPR_KWH    5
#define ENERGY_USAGE_TIMER_INTERVAL 10000
#define RESET         0

namespace Pin
{
  const uint8_t pzemTx = 8;
  const uint8_t pzemRx = 9;
  const uint8_t hc05Tx = 2;
  const uint8_t hc05Rx = 3;
  const uint8_t relaySigPin = 5;
  const uint8_t batPin = A3;
};

//Objects
SoftwareSerial pzemSerial(Pin::pzemTx,Pin::pzemRx);
SoftwareSerial hc05Serial(Pin::hc05Tx,Pin::hc05Rx);
LiquidCrystal_I2C lcd(0x27,20,4);
PZEM004Tv30 pzem(pzemSerial);
HC05 hc05(&hc05Serial);
HMI hmi(lcd);

param_t param = {0};
uint32_t prevEnergyUsageTime;
uint32_t prevGetPowerParamTime;
static uint16_t prevKWh;

void EEPROM_Write2Bytes(int address,uint16_t data)
{
  EEPROM.write(address, (data & 0xFF00) >> 8);
  EEPROM.write(address + 1,(data & 0x00FF));
}

uint16_t EEPROM_Read2Bytes(int address)
{
  return ((EEPROM[address] % 255) << 8 | EEPROM[address + 1] % 255); 
}

void setup()
{
  Serial.begin(9600);
  pinMode(Pin::relaySigPin,OUTPUT);
  param.units = EEPROM_Read2Bytes(EEPR_UNITS);
  prevKWh = EEPROM_Read2Bytes(EEPR_KWH);
  Serial.println(prevKWh);
  prevEnergyUsageTime = 0;
  prevGetPowerParamTime = 0;
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
  lcd.clear();
}


void loop()
{
  uint16_t rawVal = analogRead(Pin::batPin);
  float volt = (rawVal * 5.0) / 1024.0;
  float inputVolt = ((5.1 + 20) / 5.1) * volt;
  param.bat_level = mapFloat(inputVolt,11.0,16.5,0,99);
  hmi.Display_Control(param);
  
  //Get power parameters
  if(millis() - prevGetPowerParamTime >= 1500)
  {
    pzemSerial.listen();
    param.volt = pzem.voltage() * 100;
    param.curr = pzem.current() * 100;
    param.pwr = pzem.power() * 10;
    param.KWh = pzem.energy() * 1000;
    prevGetPowerParamTime = millis();
  }
  
  //Receive units from mobile App
  hc05Serial.listen();
  if(hc05.ReceivedData())
  {
    int RxData = hc05.DecodeData();
    Serial.print("Received: ");
    Serial.println(RxData);
    if(RxData == RESET)
    {
      //Clear the available units and reset PZEM's KWh reading
      param.units = 0;
      prevKWh = 0;
      pzem.resetEnergy();
      EEPROM_Write2Bytes(EEPR_UNITS,param.units);
      EEPROM_Write2Bytes(EEPR_KWH,prevKWh);
    }
    else
    {
      //Add received units to available units
      param.units += RxData;
      EEPROM_Write2Bytes(EEPR_UNITS,param.units);
    }
    Serial.println(param.units);
    hc05.ResetRxBuffer();
  }

  //Check if it's time to decrement available units
  if(millis() - prevEnergyUsageTime >= ENERGY_USAGE_TIMER_INTERVAL)
  {
    Serial.print("param.KWh: ");
    Serial.println(param.KWh);
    //Check if energy usage has increased by 1KWh
    if((param.KWh - prevKWh) >= 1000)
    {
      if(pzem.voltage() >= 200)
      {
        param.units--;
        prevKWh = param.KWh; 
        EEPROM_Write2Bytes(EEPR_UNITS,param.units);
        EEPROM_Write2Bytes(EEPR_KWH,prevKWh);
      }      
    }
    prevEnergyUsageTime = millis();
  }
  
  //Relay control based on available units
  digitalWrite(Pin::relaySigPin, param.units > 0 ? HIGH : LOW);
  //Overcurrent protection: turn supply off if current > 2.1A
  if(pzem.current() > 2.1)
  {
    digitalWrite(Pin::relaySigPin,HIGH);
  }
}
