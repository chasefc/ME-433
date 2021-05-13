// functions to operate the ST7789 on the PIC32
// adapted from https://github.com/sumotoy/TFT_ST7789
// and https://github.com/adafruit/Adafruit-ST7789-Library

// pin connections:
// GND - GND
// VCC - 3.3V
// SCL - B14
// SDA - B13
// RES - B15
// DC - B12
// BLK - NC

#include <xc.h>
#include "ST7789.h"
#include "spi.h"
#include "font.h"
#include "stdio.h"

void LCD_command(unsigned char com) {
    LATBbits.LATB12 = 0; // DC
    spi_io(com);
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB12 = 1; // DC
    spi_io(dat);
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB12 = 1; // DC
    spi_io(dat>>8);
    spi_io(dat);
}

void LCD_init() {
  unsigned int time = 0;
  LCD_command(ST7789_SWRESET); //software reset
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.15) {}
  
  LCD_command(ST7789_SLPOUT); //exit sleep
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.5) {}
  
  LCD_command(ST7789_COLMOD);
  LCD_data(0x55);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.01) {}
  
  LCD_command(ST7789_MADCTL);
  LCD_data(0x00);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.001) {}
  
  LCD_command(ST7789_CASET);
  LCD_data(0x00);
  LCD_data(ST7789_XSTART);
  LCD_data((240+ST7789_XSTART)>>8);
  LCD_data((240+ST7789_XSTART)&0xFF);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.001) {}

  LCD_command(ST7789_RASET);
  LCD_data(0x00);
  LCD_data(ST7789_YSTART);
  LCD_data((240+ST7789_YSTART)>>8);
  LCD_data((240+ST7789_YSTART)&0xFF);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.001) {}
  
  LCD_command(ST7789_INVON);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.01) {}

  LCD_command(ST7789_NORON);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.01) {}
  
  LCD_command(ST7789_DISPON);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.5) {}
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // should check boundary first
  LCD_setAddr(x,y,x+1,y+1);
  LCD_data16(color);
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
  LCD_command(ST7789_CASET); // Column
  LCD_data16(x0+ST7789_XSTART);
  LCD_data16(x1+ST7789_XSTART);
  
  LCD_command(ST7789_RASET); // Page
  LCD_data16(y0+ST7789_YSTART);
  LCD_data16(y1+ST7789_YSTART);

  LCD_command(ST7789_RAMWR); // Into RAM
}

void LCD_clearScreen(unsigned short color) {
  int i;
  LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
	for (i = 0;i < _GRAMSIZE; i++){
		LCD_data16(color);
	}
}

void drawChar(unsigned short x, unsigned short y, char c, unsigned short color,unsigned short b_color){
    int i;
    int j;
     if (x >240-5 | y> 240-8){
        drawString(0,0,"ERROR",BLACK,WHITE);
    }
    else{
        for (i = 0; i<5; i++){
            for (j = 0; j<8; j++){
                if ((ASCII[c-0x20][i] >> j) & 0b1 == 0b1){
                    LCD_drawPixel(x+i, y+j, color);
                }
                else {
                    LCD_drawPixel(x+i,y+j,b_color);
                }
            }
                  }
    }
    
}


void drawString(unsigned short x, unsigned short y, char * m, unsigned short color, unsigned short b_color){
    int i = 0;
    int j;
    int k;
    
    while(m[i]){ 
                    
        drawChar(x, y, m[i], color, b_color); 
        i++;
        x = x+ 6;
    }
}
void drawProgressBar(unsigned short x, unsigned short y, int length, int value, unsigned short forecolor, unsigned short backcolor){
    int i;
    int j;
    for (i = 0; i<length; i++){
        for (j = 0; j<8; j++){
            if (i<value){
                LCD_drawPixel(x+i, y+j, forecolor);
            }
            else{
                LCD_drawPixel(x+i, y+j, backcolor);
            }
        }
    }
}
void drawProgressBarPosX(unsigned short x, unsigned short y, int length, int value, unsigned short forecolor, unsigned short backcolor){
    int i;
    int j;
    for (i = 0; i<length; i++){
        for (j = 0; j<8; j++){
            if (i<value){
                LCD_drawPixel(x+i, y+j, forecolor);
            }
            else{
                LCD_drawPixel(x+i, y+j, backcolor);
            }
        }
    }
}

void drawProgressBarNegX(unsigned short x, unsigned short y, int length, int value, unsigned short forecolor, unsigned short backcolor){
    int i;
    int j;
    for (i = 0; i<length; i++){
        for (j = 0; j<8; j++){
            if (i<value){
                LCD_drawPixel(x-i, y+j, forecolor);
            }
            else{
                LCD_drawPixel(x-i, y+j, backcolor);
            }
        }
    }
}
void drawProgressBarPosY(unsigned short x, unsigned short y, int length, int value, unsigned short forecolor, unsigned short backcolor){
    int i;
    int j;
    for (i = 0; i<length; i++){
        for (j = 0; j<8; j++){
            if (i<value){
                LCD_drawPixel(x+j, y+i, forecolor);
            }
            else{
                LCD_drawPixel(x+j, y+i, backcolor);
            }
        }
    }
}
void drawProgressBarNegY(unsigned short x, unsigned short y, int length, int value, unsigned short forecolor, unsigned short backcolor){
    int i;
    int j;
    for (i = 0; i<length; i++){
        for (j = 0; j<8; j++){
            if (i<value){
                LCD_drawPixel(x+j, y-i, forecolor);
            }
            else{
                LCD_drawPixel(x+j, y-i, backcolor);
            }
        }
    }
}
