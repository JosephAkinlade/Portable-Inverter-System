#include <Arduino.h>
#include "hmi.h"

HMI::HMI(LiquidCrystal_I2C& lcdRef) : lcdRef(lcdRef)
{
  //Initialize private variables
  pageDisplayed = PAGE1;
  dispTimer = millis();
}

void HMI::ChangePageDisplayed(void)
{
  dispTimer = millis();
  pageDisplayed ^= PAGE2; 

}

void HMI::Display_Control(param_t& param)
{
  if((millis() - dispTimer) >= 10000)
  {
    ChangePageDisplayed();
    switch(pageDisplayed)
    {
      case PAGE1:
        HMI::Display_Page1(param);
        break;
      case PAGE2:
        HMI::Display_Page2(param);
        break;
    }
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
  lcdRef.clear();
  lcdRef.setCursor(0,0);
  lcdRef.print("Voltage:");
  lcdRef.setCursor(8,0);
  HMI::DisplayAlignedThreeDigits(param.volt);
  lcdRef.setCursor(0,1);
  lcdRef.print("Current:");
  lcdRef.setCursor(8,1);
  HMI::DisplayAlignedTwoDigits(param.curr);
  lcdRef.setCursor(0,2);
  lcdRef.print("Power:");
  lcdRef.setCursor(6,2);
  HMI::DisplayAlignedThreeDigits(param.pwr);
  lcdRef.setCursor(0,3);
  lcdRef.print("KwH used:");
  lcdRef.setCursor(9,3);
  HMI::DisplayAlignedThreeDigits(param.kwh); 
}

void HMI::Display_Page2(param_t& param)
{
  lcdRef.clear();
  lcdRef.setCursor(0,0);
  lcdRef.print("Units Received:");
  lcdRef.setCursor(15,0);
  HMI::DisplayAlignedThreeDigits(param.units_recvd);
  lcdRef.setCursor(0,1);
  lcdRef.print("Units left:");
  lcdRef.setCursor(11,1);
  HMI::DisplayAlignedThreeDigits(param.units_left);
  lcdRef.setCursor(0,2);
  lcdRef.print("Battery Level:"); 
  lcdRef.setCursor(14,2);
  HMI::DisplayAlignedThreeDigits(param.bat_level);
}
