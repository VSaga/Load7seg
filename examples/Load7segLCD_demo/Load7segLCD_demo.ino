#include "Load7segLCD.h"

Load7segLCD LoadLCD;

void setup() 
{
}

void loop() 
{
  int i;
  char buf[9];

  LoadLCD.clear();
  LoadLCD.write(" SAGA ");
  delay(1000);
  for(i=1;i<5;i++)
  {
    LoadLCD.radio(i);
    delay(500);
  }
  LoadLCD.battery(1);
  delay(500);
  LoadLCD.peakhold(1);
  delay(500);
  LoadLCD.unit(LCD_UNIT_LB);
  delay(500);
  LoadLCD.unit(LCD_UNIT_KG);
  delay(500);
  LoadLCD.unit(LCD_UNIT_KN);
  delay(500);
  LoadLCD.unit(LCD_UNIT_TONN);
  delay(500);
  LoadLCD.unit(LCD_UNIT_PROS);
  LoadLCD.resolution(0.1);
  for(i=0;i<=1000;i++)
  {
    LoadLCD.value(i/10.0);
    delay(10);
  }
  delay(500);
}
