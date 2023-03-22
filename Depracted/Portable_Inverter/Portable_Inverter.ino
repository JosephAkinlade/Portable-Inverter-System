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

typedef struct
{
  int voltage;
  int current;
  int power;
  int energy;
  int units_recvd;
  int units_left;
  int bat_level;
}param_t;

char RxBuffer[4] = {0};;
param_t param = {0};

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
  lcd.setCursor(8,0);
  DisplayAlignedThreeDigits(param.voltage);
  lcd.setCursor(0,1);
  lcd.print("Current:");
  lcd.setCursor(8,1);
  DisplayAlignedTwoDigits(param.current);
  lcd.setCursor(0,2);
  lcd.print("Power:");
  lcd.setCursor(6,2);
  DisplayAlignedThreeDigits(param.power);
  lcd.setCursor(0,3);
  lcd.print("KwH used:");
  lcd.setCursor(9,3);
  DisplayAlignedThreeDigits(param.energy);
}

void Display_Page1(void)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Units Received:");
  lcd.setCursor(15,0);
  DisplayAlignedThreeDigits(param.units_recvd);
  lcd.setCursor(0,1);
  lcd.print("Units left:");
  lcd.setCursor(11,1);
  DisplayAlignedThreeDigits(param.units_left);
  lcd.setCursor(0,2);
  lcd.print("Battery Level:"); 
  lcd.setCursor(14,2);
  DisplayAlignedThreeDigits(param.bat_level); 
}

static int ConvStrToInt(char* str,int len)
{//Max of 3-digit integers
  const int powerOfTen[] = {1,10,100}; 
  int integer = 0;
  for(int i = 0; i < len; i++)
  {
    integer += ((str[i]-'0')*powerOfTen[len-i-1]);
  }
  return integer;
}

static void DisplayAlignedTwoDigits(int val)
{
  if(val < 10)
  {
    lcd.print('0');
    lcd.print(val);
  }
  else
  {
    lcd.print(val);  
  }
}
static void DisplayAlignedThreeDigits(int val)
{
  if(val < 10)
  {
    lcd.print("00");
    lcd.print(val);
  }
  else if(val >= 10 && val < 100)
  {
    lcd.print('0');
    lcd.print(val);
  }
  else
  {
    lcd.print(val);
  }  
}

void Update_Units(char* str)
{
  param.units_recvd = ConvStrToInt(str, strlen(str));
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Units Received:");
  lcd.setCursor(15,0);
  DisplayAlignedThreeDigits(param.units_recvd);
  lcd.setCursor(0,1);
  lcd.print("Units left:");
  lcd.setCursor(11,1);
  DisplayAlignedThreeDigits(param.units_left);
  lcd.setCursor(0,2);
  lcd.print("Battery Level:"); 
  lcd.setCursor(14,2);
  DisplayAlignedThreeDigits(param.bat_level);
  memset(RxBuffer, 0, 4);
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
  if(HC05.available())
  {
    while(HC05.available())
    {
      HC05.readBytes(RxBuffer, 5);
    }
    Update_Units(RxBuffer);    
  }
}
