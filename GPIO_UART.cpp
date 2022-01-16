
#include "GPIO_UART.h"
#include <cmath>
#include <cstdio>

void GPIO_UART::readBit() {
    
    int dataBit = rx.read();
        

    if (rxFrame.state.dataLen != 0) {
        rxFrame.state.dataLen--;
        rxFrame.dataByte >>= 1;
        rxFrame.dataByte |= dataBit ? 0x80 : 0x00;

        if (rxFrame.state.parityLen) {
            rxFrame.state.parity ^= dataBit;
            
        }
    }
    else if (rxFrame.state.parityLen) {
        rxFrame.state.parityLen--;
        if (dataBit != (rxFrame.state.parity ^ rxFrame.state.parity_even)) {
            resetRead();
            return;
             
        }
    }
    else if (rxFrame.state.stopLen) {
        rxFrame.state.stopLen--;

        if (dataBit != 1) {
            return;
           
        }
             
        if (rxFrame.state.stopLen == 0) {

            uint8_t wPointer = rxFrame.wPointer++;

            rxFrame.wPointer = rxFrame.wPointer < sizeof(rxFrame.buffer) ? rxFrame.wPointer : 0;
            
            if (rxFrame.wPointer == rxFrame.rPointer) {
                rxFrame.wPointer = wPointer;

            }
            rxFrame.buffer[wPointer] = rxFrame.dataByte;
            resetRead();           
            return;
        }
    }
   

    rxClock.attach_us(callback(this, &GPIO_UART::readBit),boudRateUs);    
    
    return;

};

void GPIO_UART::format( unsigned int dataLen,
                        unsigned int stopLen,
                        unsigned int parityLen,
                        bool parity_even)
{

     uartSettings.dataLen = dataLen;
     uartSettings.stopLen = stopLen;
     uartSettings.parityLen = parityLen;
     uartSettings.parity_even = parity_even;

    setupUart();

    
    
};
void GPIO_UART::setupUart(){
    rxFrame.state.dataLen   = uartSettings.dataLen;
    rxFrame.state.parityLen = uartSettings.parityLen;
    rxFrame.state.stopLen   = uartSettings.stopLen;
    rxFrame.state.parity_even = uartSettings.parity_even;

    txFrame.state.dataLen   = uartSettings.dataLen;
    txFrame.state.parityLen = uartSettings.parityLen;
    txFrame.state.stopLen   = uartSettings.stopLen;
    txFrame.state.parity_even = uartSettings.parity_even;

}


int GPIO_UART::read() {
 
    if (rxFrame.rPointer == rxFrame.wPointer) {
            return -1;
        }

    int c = rxFrame.buffer[rxFrame.rPointer++];
    rxFrame.rPointer = rxFrame.rPointer < sizeof(rxFrame.buffer) ? rxFrame.rPointer : 0;
    
    return c;
};


void GPIO_UART::resetRead(){
    rxInterrupt.enable_irq();
    rxFrame.state.isBusy = 0;

}


void GPIO_UART::initUart(){

    

    if (rxFrame.state.isBusy == 0) {
        rxInterrupt.disable_irq();    
        rxFrame.state.isBusy = 1;
        rxFrame.state.dataLen   = uartSettings.dataLen;
        rxFrame.state.parityLen = uartSettings.parityLen;
        rxFrame.state.stopLen   = uartSettings.stopLen;

        rxClock.attach_us(callback(this,&GPIO_UART::readBit),boudRateUs);
    }

};

void GPIO_UART::sendBit() {


   


    int dataBit;

    if (txFrame.state.dataLen) {



        txFrame.state.dataLen--;
        dataBit = txFrame.dataByte & 0x01;
        txFrame.dataByte >>= 1;

        if (txFrame.state.parityLen) {
            txFrame.state.parity ^= dataBit;
        }
    }
    else if (txFrame.state.parityLen) {
        txFrame.state.parityLen--;
        dataBit = txFrame.state.parity ^ txFrame.state.parity_even;
    }
    else if (txFrame.state.stopLen) {
        txFrame.state.stopLen--;
        dataBit = 1;
        

    }
    else {

        if (txFrame.rPointer == txFrame.wPointer) {
            txFrame.state.isBusy = 0;

            return;
        }


        txFrame.dataByte = txFrame.buffer[txFrame.rPointer++];
        txFrame.rPointer = txFrame.rPointer < sizeof(txFrame.buffer) ? txFrame.rPointer : 0;
        
        txFrame.state.dataLen   = uartSettings.dataLen;
        txFrame.state.parityLen = uartSettings.parityLen;
        txFrame.state.stopLen   = uartSettings.stopLen;

        dataBit = 0;
    }

    tx.write(dataBit);
   

    
    txClock.attach_us(callback(this,&GPIO_UART::sendBit),boudRateUs);
    
   
};

int GPIO_UART::write(uint8_t c) {



    uint8_t wPointer = txFrame.wPointer++;
    
    txFrame.wPointer = txFrame.wPointer < sizeof(txFrame.buffer) ? txFrame.wPointer : 0;
    
    if (txFrame.wPointer == txFrame.rPointer) {
        txFrame.wPointer = wPointer;

    }
    txFrame.buffer[wPointer] = c;



    if (txFrame.state.isBusy == 0) {
        txFrame.state.isBusy = 1;
        

       
        sendBit();

        
    }
   
    return c;

};


GPIO_UART::GPIO_UART(PinName rxPin, PinName txPin): rxInterrupt(rxPin),tx(txPin),rx(rxPin)
{
    tx.write(1);
    rxFrame.state.isBusy = 0;
    txFrame.state.isBusy = 0;  
    rxInterrupt.fall(callback(this,&GPIO_UART::initUart));

};

void GPIO_UART::run(int boudRate){

    boudRateUs = floor(1000000/boudRate) - 21;
   
};







