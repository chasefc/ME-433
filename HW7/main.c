#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "math.h"

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
#pragma config WDTPS = PS1048576 // use largest wdt
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

void initSPI();
unsigned short create_byte(unsigned char channel, unsigned short voltage);
unsigned char spi_io(unsigned char o);


int main(void){
    __builtin_enable_interrupts();
    
    initSPI();
    
    int sign = 1;
    int j = 0; // triangle
    
    while(1){
        // write one byte over SPI1
        
        int i;
        for (i = 0; i<628; i++){
            _CP0_SET_COUNT(0);
             // make 2Hz sin wave
            unsigned short v_sin = 450*sin(i/100.0) + 511.5; //scale down a touch so the ends dont clip
            unsigned short p = create_byte(0, v_sin);

            LATAbits.LATA3 = 0; // bring CS low
            spi_io(p>>8);
            spi_io(p);
            LATAbits.LATA3 = 1; // bring CS high
            
            unsigned short v1 = (short) j*(900./628.0); // scaled down a touch because end behavior is a little weird
            if (sign == 1){
                j++;
            }
            else j--;
            
            unsigned short p1 = create_byte(1, v1);
            
            LATAbits.LATA3 = 0; // bring CS low
            spi_io(p1>>8);
            spi_io(p1);
            LATAbits.LATA3 = 1; // bring CS high
            
            while(_CP0_GET_COUNT() < 48000000/(2*1256)){ // wait 
                ;
            }
        }
        sign = -1*sign;
    }
}

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    // Turn off analog pins
    ANSELA = 0; 
    // Make an output pin for CS
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 1; 
    // Set A1 SDO1
    RPA1Rbits.RPA1R = 0b0011;
    // Set B5 SDI1
    SDI1Rbits.SDI1R = 0b0001;

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi
}

unsigned short create_byte(unsigned char channel, unsigned short voltage){
    // generate output p
    unsigned short p;
    p = 0b0000000000000000;
    p = p|(channel << 15);
    p = p|(0b111<<12);
    p = p|(voltage<<2);
    return p;
}


// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}