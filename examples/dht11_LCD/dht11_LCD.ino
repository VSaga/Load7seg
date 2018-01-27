#include <DHT11.h> //DHT11 library by dalxx
#include "Load7segLCD.h"

Load7segLCD LoadLCD;

int pin=2;
DHT11 dht11(pin); 

void setup()
{
    LoadLCD.clear();
    LoadLCD.write(" SAGA ");
    delay(500);
}

void loop()
{
  char buf[9];
  int err;
  float temp, humi;
  
  if((err=dht11.read(humi, temp))==0)
  {
    LoadLCD.unit(LCD_UNIT_DEG_C);
    LoadLCD.resolution(0.5);  
    LoadLCD.value(temp);
    delay(1000);
    LoadLCD.unit(LCD_UNIT_PROS);
    LoadLCD.resolution(0.5);  
    LoadLCD.value(humi);
  }
  else
  {
    LoadLCD.unit(LCD_UNIT_NONE);
    LoadLCD.clear();
    snprintf(buf, 7, "Err. %d", err);
    LoadLCD.write(buf);
  }
  delay(DHT11_RETRY_DELAY); //delay for reread
}



