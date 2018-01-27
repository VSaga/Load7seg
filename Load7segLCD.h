/*
   Load7segLCD.h - Library for flashing Morse code.
   Created by Vegar Bokeli Saga Dec 2017.

 */
#ifndef Load7segLCD_h
#define Load7segLCD_h

#include <Arduino.h>

#define LCD_UNIT_NONE      0x0
#define LCD_UNIT_LB        0x01
#define LCD_UNIT_KG        0x02
#define LCD_UNIT_KN        0x03
#define LCD_UNIT_TONN      0x04
#define LCD_UNIT_DEG       0x06
#define LCD_UNIT_DEG_C     0x07
#define LCD_UNIT_DEG_F     0x08
#define LCD_UNIT_PROS      0x09


class Load7segLCD
{
  public:
    Load7segLCD();
    void init(void);
    void clear(void);
    void write(const char *text);
    void value(float value);
    void sendToLcd(void);
    void radio(byte strength);// values 0-4 segments
    void unit(byte unit);
    void battery(byte on);
    void peakhold(byte on);
    void resolution(float resolution);
   private:
    void unitseg(byte seg);
    char updateunit(void);
    unsigned char buf[8];// Hold the LCD data (6 segments * 1 bytes per segment and 2 bytes for radio, battery and units, means only the first 8 bytes are used)
    byte set_bankselect; 
    byte set_modeset;
    byte set_deviceselect;      
    byte set_blink;
    byte set_datapointer;
    int _PowerPin;
    byte _unit;
    float _resolution;
};

#endif
