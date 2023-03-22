#pragma once 

#include <LiquidCrystal_I2C.h>

typedef struct
{
  int volt;
  int curr;
  int pwr;
  int kwh;
  int units_recvd;
  int units_left;
  int bat_level;
}param_t;

class HMI
{
  private:
  LiquidCrystal_I2C* lcdPtr;
  uint8_t pageDisplayed;
  uint32_t dispTimer;
  void DisplayAlignedTwoDigits(int val);
  void DisplayAlignedThreeDigits(int val);
  void ChangePageDisplayed(void);
  enum DISPLAY_PAGES{PAGE1 = 0, PAGE2};

  public:
    HMI(LiquidCrystal_I2C* lcdPtr);
    void Display_Page1(param_t& param);
    void Display_Page2(param_t& param);
    void Display_Control(param_t& param);
};
