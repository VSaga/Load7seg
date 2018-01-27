/*
   Morse.cpp - Library for flashing Morse code.
   Created by David A. Mellis, November 2, 2007.
   Released into the public domain.
 */

#include "Arduino.h"
#include "Load7segLCD.h"
#include <Wire.h>

#define NEXTCMD 128     // Issue when there will be more commands after this one
#define LASTCMD 0       // Issue when when this is the last command before ending transmission

/* Constants and default settings for the PCF */
// MODE SET
#define MODESET 64

#define MODE_NORMAL 0
#define MODE_POWERSAVING 16

#define DISPLAY_DISABLED 0
#define DISPLAY_ENABLED 8

#define BIAS_THIRD 0
#define BIAS_HALF 4

#define DRIVE_STATIC 1
#define DRIVE_2 2
#define DRIVE_3 3
#define DRIVE_4 0

//BLINK
#define BLINK  112

#define BLINKING_NORMAL 0
#define BLINKING_ALTERNATION 4

#define BLINK_FREQUENCY_OFF 0
#define BLINK_FREQUENCY2 1
#define BLINK_FREQUENCY1 2
#define BLINK_FREQUENCY05 3

//LOADDATAPOINTER
#define LOADDATAPOINTER  0

//BANK SELECT
#define BANKSELECT 120

#define BANKSELECT_O1_RAM0 0
#define BANKSELECT_O1_RAM2 2

#define BANKSELECT_O2_RAM0 0
#define BANKSELECT_O2_RAM2 1

#define DEVICE_SELECT 96
#define PCF8576 B111000   // This is the address of the PCF on the i2c bus

#define LCD_I2C_ADDR      0x70
#define LCD_CMD_MODE      0x5C
#define LCD_CMD_BLINKON   0x71
#define LCD_CMD_BLINKOFF  0x70
#define LCD_CMD_MORE      0x80
#define LCD_ADDR_7SEG     0x00
#define LCD_ADDR_UNITS    0x0C
#define LCD_ADDR_RADIO    0x0E

#define LCD_SEG_LB        0x02
#define LCD_SEG_KG        0x20
#define LCD_SEG_KN        0x40
#define LCD_SEG_TONN      0x80
#define LCD_SEG_PH        0x04
#define LCD_SEG_BAT       0x08
#define LCD_SEG_RADIO1    0x10
#define LCD_SEG_RADIO2    0x20
#define LCD_SEG_RADIO3    0x40
#define LCD_SEG_RADIO4    0x80
//7-seg:
#define LCD_SEG_DOT       0x01
#define LCD_SEG_NEG       0x40
#define LCD_SEG_SPACE     0x00
#define LCD_SEG_DEGR      0xCC
#define LCD_SEG_A         0xEE
#define LCD_SEG_C         0xB8
#define LCD_SEG_b         0xF2
#define LCD_SEG_E         0xF8
#define LCD_SEG_d         0x76
#define LCD_SEG_F         0xE8
#define LCD_SEG_G         0xFA
#define LCD_SEG_H         0xE6
#define LCD_SEG_i         0xA0
#define LCD_SEG_J         0x36
#define LCD_SEG_L         0xB0
#define LCD_SEG_n         0x62
#define LCD_SEG_o         0x72
#define LCD_SEG_P         0xEC
#define LCD_SEG_R         LCD_SEG_o&LCD_SEG_F
#define LCD_SEG_S         0xDA
#define LCD_SEG_t         0xF0
#define LCD_SEG_U         0xB6
#define LCD_SEG_Y         0xC6
#define LCD_SEG__         0x10

const unsigned char  LCD_SEG_7SEG[]={LCD_SEG_SPACE,0xBE,0x06,0x7C,0x5E,0xC6,0xDA,0xF2,0x0E,0xFE,0xCE,
                                    LCD_SEG_A,LCD_SEG_b,LCD_SEG_C,LCD_SEG_d,LCD_SEG_E,LCD_SEG_F,
                                    LCD_SEG_G,LCD_SEG_H,LCD_SEG_i,LCD_SEG_J,LCD_SEG_L,LCD_SEG_n,LCD_SEG_o,
                                    LCD_SEG_P,LCD_SEG_R, LCD_SEG_S,LCD_SEG_t,LCD_SEG_U,LCD_SEG_Y,LCD_SEG__,
                                    LCD_SEG_NEG,LCD_SEG_DEGR};
const unsigned char  LCD_SEG_RADIO[]={0x0,0x10,0x30,0x70,0xF0};
/* Text to LCD segment mapping. You can add your own symbols, but make sure the index and font arrays match up */
const char LCD_SEG_INDEX[] = " 0123456789ABCDEFGHIJLNOPRSTUY_-°";


Load7segLCD::Load7segLCD()
{
  Wire.begin();
  this->set_bankselect = BANKSELECT | BANKSELECT_O1_RAM0 | BANKSELECT_O2_RAM0; 
  this->set_modeset = MODESET | MODE_POWERSAVING | DISPLAY_ENABLED | BIAS_THIRD | DRIVE_4; // default init mode
  this->set_deviceselect = DEVICE_SELECT;      
  this->set_blink = BLINK | BLINKING_ALTERNATION | BLINK_FREQUENCY_OFF; // Whatever you do, do not blink. 
  this->set_datapointer = LOADDATAPOINTER | 0;
  memset(this->buf, 0xFF, 8);//All segments on
  _resolution=0.3;
  //_prec=1;
}

void Load7segLCD::init()
{
  Wire.beginTransmission(PCF8576);
  Wire.write(NEXTCMD | this->set_modeset); 
  Wire.write(NEXTCMD | this->set_deviceselect); 
  Wire.write(NEXTCMD | this->set_blink); 
  Wire.write(LASTCMD | this->set_datapointer); 
  Wire.write(this->buf, 8);
  Wire.endTransmission();  
}

/* Physically send out the given data */
void Load7segLCD::sendToLcd() 
{
  Wire.beginTransmission(PCF8576);
  Wire.write(NEXTCMD | this->set_modeset); 
  Wire.write(NEXTCMD | this->set_deviceselect); 
  Wire.write(NEXTCMD | this->set_blink); 
  Wire.write(LASTCMD | this->set_datapointer); 
  Wire.write(this->buf, 8);                     // Store all matrix bytes
  Wire.endTransmission();    
}

void Load7segLCD::clear(void)
{
  memset(this->buf, 0, 8);//All segments off
  this->sendToLcd();
}

/* Writes the first 8 characters of the string to the matrix */
void Load7segLCD::write(const char *text) 
{
  char textIndex=0;

  for (int idx=0; idx<6; idx++)
  {  
    if(text[textIndex]==0)
       break;
    char *c = strchr(LCD_SEG_INDEX, (int)toupper(text[textIndex]));
    int pos;
    if (c == NULL) 
      pos = 0;      // Char not found, use underscore space instead
    else 
      pos = c - LCD_SEG_INDEX;
    buf[idx] = LCD_SEG_7SEG[pos];
    textIndex++;
    if(text[textIndex]=='.')
    {
      buf[idx]|= LCD_SEG_DOT;
      textIndex++;
    }
  }
  this->sendToLcd();
}

void Load7segLCD::value(float value)
{
  char buf[8],with,prec;

  memset(this->buf, 0, 6);//All 7-segments off
  with=this->updateunit();
  value=(value/this->_resolution);
  value=(long)(value);
  value=value*this->_resolution;
  
  prec=ceil(log10(1.0/this->_resolution));
  if(prec<0) prec=0;
  
  if(prec)
    with++; //Makee room for '.'
  dtostrf(value, with, prec, buf);
  this->write(buf);
}

void Load7segLCD::resolution(float resolution)
{
  _resolution=resolution;
}

void Load7segLCD::radio(byte strength)// values 0-4 segments
{
  if(strength<5)
    this->buf[7]=LCD_SEG_RADIO[strength];
  this->sendToLcd();
}

void Load7segLCD::unit(byte unit)
{
  _unit=unit;
  updateunit();
  this->sendToLcd();
}

char Load7segLCD::updateunit()
{
  char *unitstr;
  
  switch(_unit)
  {
    case LCD_UNIT_LB: 
        this->unitseg(LCD_SEG_LB);
        break;
    case LCD_UNIT_KG:
        this->unitseg(LCD_SEG_KG);
        break;
    case LCD_UNIT_KN:
        this->unitseg(LCD_SEG_KN);
        break;
    case LCD_UNIT_TONN:
        this->unitseg(LCD_SEG_TONN);
        break;
    default:
        this->unitseg(0);
        break;
  }
  switch(_unit)
  {
    case LCD_UNIT_DEG:
        buf[5]=LCD_SEG_DEGR;
        return 5;
    case LCD_UNIT_DEG_C:
        buf[4]=LCD_SEG_DEGR;
        buf[5]=LCD_SEG_C;
        return 4;
    case LCD_UNIT_DEG_F:
        buf[4]=LCD_SEG_DEGR;
        buf[5]=LCD_SEG_F;
        return 4;
    case LCD_UNIT_PROS:
        buf[4]=LCD_SEG_DEGR;
        buf[5]=LCD_SEG_o;
        return 4;
  }
  return 6;
}

void Load7segLCD::unitseg(byte seg)
{
  this->buf[6]&=~(LCD_SEG_KG|LCD_SEG_KN|LCD_SEG_TONN|LCD_SEG_LB);
  this->buf[6]|=(LCD_SEG_KG|LCD_SEG_KN|LCD_SEG_TONN|LCD_SEG_LB)&seg;
}

void Load7segLCD::battery(byte on)
{
  this->buf[6]&=~LCD_SEG_BAT;
  if(on)
    this->buf[6]|=LCD_SEG_BAT;
  this->sendToLcd();
}

void Load7segLCD::peakhold(byte on)
{
  this->buf[6]&=~LCD_SEG_PH;
  if(on)
    this->buf[6]|=LCD_SEG_PH;
  this->sendToLcd();
}

