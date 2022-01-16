#include "ThisThread.h"
#include "mbed.h"
#include "GPIO_UART.h"

int main()
{
    
    static GPIO_UART uart(USBRX,USBTX); 
    //static GPIO_UART uart(PB_1,PB_2); 

    uart.format(
        /* dataLen */ 8,
        /* stopBit */ 1,
        /* parityLen */ 0,
        /* even */ 0
    );
    uart.run(38400);
       

    while (true) {
            
       int i = uart.read();

       if(i!= -1){
        
        
            uart.write(i);
        //printf("The time taken was %i seconds\n", uart.time);
       
        
       }
       ThisThread::sleep_for(10);
    // printf("%i\n",uart.boudRateUs);
     
    }
}
