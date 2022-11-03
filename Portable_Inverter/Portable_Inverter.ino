#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"

/**
 * Pinouts
 * SCL -- A4
 * SDA -- A5
*/

SoftwareSerial HC05(2,3); //rx,tx

LiquidCrystal_I2C lcd(0x27,20,4);

enum DISPLAY_PAGES
{
  PAGE0 = 0,
  PAGE1
};

int pageDisplayed = PAGE0;
static uint32_t dispTimer;

void ChangePageDisplayed(void)
{
  dispTimer = millis();
  pageDisplayed ^= PAGE1; 

}

void Display_Page0(void)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Voltage:");
  lcd.setCursor(0,1);
  lcd.print("Current:");
  lcd.setCursor(0,2);
  lcd.print("Power:");
  lcd.setCursor(0,3);
  lcd.print("KwH used:");
}

void Display_Page1(void)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Total Units:");
  lcd.setCursor(0,1);
  lcd.print("Units left:");  
}

void Display_Control(void)
{
  if((millis() - dispTimer) >= 10000)
  {
    ChangePageDisplayed();
    switch(pageDisplayed)
    {
      case PAGE0:
        Display_Page0();
        break;
      case PAGE1:
        Display_Page1();
        break;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  HC05.begin(9600);
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  Display_Page0();
}


void loop()
{
  Display_Control();
  while(HC05.available())
  {
    Serial.print(HC05.read());
  }

}
