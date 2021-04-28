#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "i2c_master_noint.h" 

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
void setPin(unsigned char address, unsigned char reg, unsigned char value);
unsigned char readPin(unsigned char waddress, unsigned char raddress, unsigned char reg);


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

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4=0;
    TRISBbits.TRISB4=1;
    LATAbits.LATA4=0;
    
    //I2C chip initialization 
    i2c_master_setup();
    setPin(0b01000000,0x00, 0x00); //set IODIRA
    setPin(0b01000000,0x01,0xFF);  // set ODIRB
    
       
    while (1) {
        LATAINV=0b10000;
        delay(); //wait half a second 
        
        if(readPin(0b1000000, 0b1000001, 0x19) == 0){
            setPin(0b01000000,0x0A,0b10000000); //set OLATA GPA7 pin high
        }
        else {
            setPin(0b01000000,0x0A,0b00000000); //set OLATA GPA7 pin high

        }

    }           
       
 }


void delay(void){
    float time = 0.05;
    float clocks= time/(1.0/24000000.0);
   
 _CP0_SET_COUNT(0); //sets core timer to 0 once we push button
    while(_CP0_GET_COUNT() < clocks){
        ;
    }
}

void setPin(unsigned char address, unsigned char reg, unsigned char value){
       i2c_master_start();
       i2c_master_send(address); //send write address
       i2c_master_send(reg); //send OLATA register
       i2c_master_send(value); // send value
       i2c_master_stop(); 
     }

unsigned char readPin(unsigned char waddress, unsigned char raddress, unsigned char reg){
    i2c_master_start();
    i2c_master_send(waddress);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(raddress);
    unsigned char value = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    return value;
                
}





