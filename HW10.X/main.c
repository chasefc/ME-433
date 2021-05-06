#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "ws2812b.h"
// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use internal oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // internal RC
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations


void delay(void);

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    ws2812b_setup();
    TRISAbits.TRISA4=0;
    LATAbits.LATA4=0;
    
    while (1){
    int numLEDs = 4;
    wsColor c[numLEDs];
       
        c[0].r = 0;
        c[0].g = 0;
        c[0].b = 255;
        
        c[1].r = 0;
        c[1].g = 255;
        c[1].b = 0;
        
        c[2].r = 255;
        c[2].g = 0;
        c[2].b = 0;
        
        c[3].r = 255;
        c[3].g = 0;
        c[3].b = 0;
        
        c[4].r = 255;
        c[4].g = 0;
        c[4].b = 0;       
    /*int i=0;
    int j=90;
    int k=180;
    int l=270;
    

    int numLEDs = 4;
        wsColor c[numLEDs];
        c[0]=HSBtoRGB(i,1,1);
        c[1]=HSBtoRGB(j,1,1);
        c[2]=HSBtoRGB(k,1,1);
        c[3]=HSBtoRGB(l,1,1);
        i++;
        j++;
        k++;
        l++;
               
        if (i=361){
            i=0;
        }
        else if(j=361){
            j=0;
        }
        else if(k=361){
            k=0;
        }
        else if(l=361){
            l=0;
        }*/
        
      

  ws2812b_setColor(c, 4);
          
        LATAINV=0b10000;
        delay(); //blink 
        
            }           
       
}

void delay(void){
    float time = 0.005;
    float clocks= time/(1.0/24000000.0);
   
 _CP0_SET_COUNT(0); //sets core timer to 0 once we push button
    while(_CP0_GET_COUNT() < clocks){
        ;
    }
}







