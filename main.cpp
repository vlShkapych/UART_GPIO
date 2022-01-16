#include "ThisThread.h"
#include "mbed.h"
#include "GPIO_UART.h"

int main()
{
    
    static GPIO_UART uart(USBRX,USBTX); 
   

    uart.format(
        /* dataLen */ 8,
        /* stopBit */ 1,
        /* parityLen */ 0,
        /* even */ 0
    );
    uart.run(9600);
       

    while (true) {
            
       int i = uart.read();

       if(i!= -1){
        
            uart.write(i);
        
       }
       ThisThread::sleep_for(10);
    
    }
}
