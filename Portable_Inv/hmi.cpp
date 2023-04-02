#include <Arduino.h>
#include "hmi.h"

HMI::HMI(LiquidCrystal_I2C& lcdRef) : lcdRef(lcdRef)
{
  //Initialize private variables
  pageDisplayed = (uint8_t)PAGE2;
  dispTimer = millis();
}

void HMI::Display_Control(param_t& param)
{   
  switch(HMI::pageDisplayed)
  {
    case PAGE1:
      HMI::Display_Page1(param);
      if(millis() - dispTimer >= 4000)
      {
        lcdRef.clear();
        pageDisplayed = PAGE2;
        dispTimer = millis();
      }
      break;
    case PAGE2:
      HMI::Display_Page2(param);
      if(millis() - dispTimer >= 4000)
      {
        lcdRef.clear();
        pageDisplayed = PAGE1;
        dispTimer = millis();
      }
      break;
  }
}

void HMI::DisplayAlignedTwoDigits(int val)
{
  if(val < 10)
  {
    lcdRef.print('0');
    lcdRef.print(val);
  }
  else
  {
    lcdRef.print(val);  
  }
}

void HMI::DisplayAlignedThreeDigits(int val)
{
  if(val < 10)
  {
    lcdRef.print("00");
    lcdRef.print(val);
  }
  else if(val >= 10 && val < 100)
  {
    lcdRef.print('0');
    lcdRef.print(val);
  }
  else
  {
    lcdRef.print(val);
  }  
}

void HMI::Display_Page1(param_t& param)
{
  lcdRef.setCursor(0,0);
  lcdRef.print("Voltage(V): ");
  lcdRef.print(param.volt / 100.0);
  lcdRef.setCursor(0,1);
  lcdRef.print("Current(A): ");
  lcdRef.print(param.curr / 100.0);
  lcdRef.setCursor(0,2);
  lcdRef.print("Power(W): ");
  lcdRef.print(param.pwr / 10.0);
  lcdRef.setCursor(0,3);
  lcdRef.print("Energy(KWh): ");
  lcdRef.print(param.KWh / 1000.0); 
}

void HMI::Display_Page2(param_t& param)
{
  lcdRef.setCursor(0,0);
  lcdRef.print("Units(KWh): ");
  HMI::DisplayAlignedThreeDigits(param.units);
  lcdRef.setCursor(0,1);
  lcdRef.print("Battery(%): "); 
  HMI::DisplayAlignedThreeDigits(param.bat_level);
}
