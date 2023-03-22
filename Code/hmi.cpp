#include <Arduino.h>
#include "hmi.h"

HMI::HMI(LiquidCrystal_I2C* lcdPtr)
{
  //Initialize private variables
  this->lcdPtr = lcdPtr;
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
    lcdPtr->print('0');
    lcdPtr->print(val);
  }
  else
  {
    lcdPtr->print(val);  
  }
}

void HMI::DisplayAlignedThreeDigits(int val)
{
  if(val < 10)
  {
    lcdPtr->print("00");
    lcdPtr->print(val);
  }
  else if(val >= 10 && val < 100)
  {
    lcdPtr->print('0');
    lcdPtr->print(val);
  }
  else
  {
    lcdPtr->print(val);
  }  
}

void HMI::Display_Page1(param_t& param)
{
  lcdPtr->clear();
  lcdPtr->setCursor(0,0);
  lcdPtr->print("Voltage:");
  lcdPtr->setCursor(8,0);
  HMI::DisplayAlignedThreeDigits(param.volt);
  lcdPtr->setCursor(0,1);
  lcdPtr->print("Current:");
  lcdPtr->setCursor(8,1);
  HMI::DisplayAlignedTwoDigits(param.curr);
  lcdPtr->setCursor(0,2);
  lcdPtr->print("Power:");
  lcdPtr->setCursor(6,2);
  HMI::DisplayAlignedThreeDigits(param.pwr);
  lcdPtr->setCursor(0,3);
  lcdPtr->print("KwH used:");
  lcdPtr->setCursor(9,3);
  HMI::DisplayAlignedThreeDigits(param.kwh); 
}

void HMI::Display_Page2(param_t& param)
{
  lcdPtr->clear();
  lcdPtr->setCursor(0,0);
  lcdPtr->print("Units Received:");
  lcdPtr->setCursor(15,0);
  HMI::DisplayAlignedThreeDigits(param.units_recvd);
  lcdPtr->setCursor(0,1);
  lcdPtr->print("Units left:");
  lcdPtr->setCursor(11,1);
  HMI::DisplayAlignedThreeDigits(param.units_left);
  lcdPtr->setCursor(0,2);
  lcdPtr->print("Battery Level:"); 
  lcdPtr->setCursor(14,2);
  HMI::DisplayAlignedThreeDigits(param.bat_level);
}
