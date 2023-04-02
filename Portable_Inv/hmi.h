#pragma once 

#include <LiquidCrystal_I2C.h>

typedef struct
{
  uint16_t volt;
  uint16_t curr;
  uint16_t pwr;
  uint16_t KWh;
  uint16_t units;
  uint16_t bat_level;
}param_t;

class HMI
{
  private:
    LiquidCrystal_I2C& lcdRef;
    uint8_t pageDisplayed;
    uint32_t dispTimer;
    void DisplayAlignedTwoDigits(int val);
    void DisplayAlignedThreeDigits(int val);
    void ChangePageDisplayed(void);
    enum DISPLAY_PAGES{PAGE1 = 1, PAGE2 = 2};

  public:
    HMI(LiquidCrystal_I2C& lcdRef);
    void Display_Page1(param_t& param);
    void Display_Page2(param_t& param);
    void Display_Control(param_t& param);
};
