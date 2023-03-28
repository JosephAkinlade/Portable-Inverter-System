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
#include "hmi.h"
#include "hc05.h"

#define EEPROM_UNITS  2
#define ENERGY_USAGE_TIMER_INTERVAL 10000
#define RESET         0

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
uint32_t prevEnergyUsageTime;
uint32_t prevGetPowerParamTime;
static uint16_t prevKWh;

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
  param.units = EEPROM[EEPROM_UNITS];
  prevKWh = 0;
  prevEnergyUsageTime = 0;
  prevGetPowerParamTime = 0;
  hmi.Display_Page1(param);
}


void loop()
{
  hmi.Display_Control(param);
  
  //Get power parameters
  if(millis() - prevGetPowerParamTime >= 1500)
  {
    pzemSerial.listen();
    param.volt = pzem.voltage();
    param.curr = pzem.current();
    param.pwr = pzem.power();
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
      //Clear the available units
      param.units = 0;
      //Add code to clear the KWh reading of the PZEM module
      EEPROM.write(EEPROM_UNITS,param.units);
    }
    else
    {
      //Add received units to available units
      param.units += RxData;
      EEPROM.write(EEPROM_UNITS,param.units);
    }
    Serial.println(param.units);
    hc05.ResetRxBuffer();
  }

  //Check if it's time to decrement available units
  if(millis() - prevEnergyUsageTime >= ENERGY_USAGE_TIMER_INTERVAL)
  {
    //Check if energy usage has increased by 1KWh
    if((param.KWh - prevKWh) >= 1000)
    {
      param.units--;
      EEPROM.write(EEPROM_UNITS,param.units);
      prevKWh = param.KWh; 
    }
    prevEnergyUsageTime = millis();
  }
  
  //Relay control based on available units
  digitalWrite(Pin::relaySigPin, param.units > 0 ? HIGH : LOW);
}
