
#ifndef GPIO_UART_H
#define GPIO_UART_H

#include "mbed.h"
#include <queue>
#include "DigitalIn.h"
#include "HighResClock.h"
#include "PinNames.h"
#include "Timeout.h"
#include "mbed.h"
#include <deque>
#include <math.h>
#include "InterruptIn.h"
#include "Timeout.h"
#include <cstdio>


struct DataFrame{

            uint8_t buffer[32];
            int rPointer;
            int wPointer;
            uint8_t dataByte;


            volatile struct {
                bool isBusy:1;

                unsigned int dataLen=8;
                unsigned int stopLen = 1;
                unsigned int parityLen=1;

                bool parity_even;
                unsigned int parity;
        
            } state;
};

class GPIO_UART {

    public:
       
        unsigned int boudRate = 9600; 

        Timeout rxClock;
        Timeout txClock;

        DigitalOut tx;
        DigitalIn rx;
        InterruptIn rxInterrupt;

        Timer t;
        int time;
        
       

        struct DataFrame rxFrame;
        struct DataFrame txFrame;

        struct {
            unsigned int dataLen = 0;
            unsigned int stopLen = 0;
            unsigned int parityLen;
            bool parity_even;
        } uartSettings;

        GPIO_UART(PinName rxPin, PinName txPin);
        
        void initUart();

        void run(int boudRate);
        void readBit();
        void sendBit();
        void resetRead();
        void setupUart();
 
        int read();
        void format(unsigned int dataLen=8,
                    unsigned int stopLen = 1,
                    unsigned int parityLen=1,
                    bool parity_even = 0);

        int write(uint8_t chr);


};

#endif