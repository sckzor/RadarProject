#include <stdint.h>

#include "addrmap/io.h"
#include "addrmap/resets.h"
#include "addrmap/sio.h"

// Put a 32 bit word in a position in memory
void PUT32(void* a, void* b) {
	asm("str %r1, [r0];"
	    "bx  %lr;"
	    : "=r" (b)
	    : "r" (a)
	    : "%r1");
}


// Get a 32 bit word from a position in memory
uint32_t GET32(void* a) {
	asm("ldr %r0, [r0];"
	    "bx  %lr;"
	    : "=r" (a)
	    : "r" (a)
	    : "%r0");
}

// Wait for number of CPU cycles
void DELAY(uint32_t a) {
	asm("sub %r0, #1;"
	    "bne DELAY;"
	    "bx  lr"
	    : "=r" (a)
	    : "r" (a)
	    : "%r0");
}


int blink(void)
{
    //release reset on IO_BANK0
    PUT32(RESETS_RESET_CLR,1<<5); //IO_BANK0
    //wait for reset to be done
    while(1)
    {
        if((GET32(RESETS_RESET_DONE_RW)&(1<<5))!=0) break;
    }

    //output disable
    PUT32(SIO_GPIO_OE_CLR,1<<25);
    //turn off pin 25
    PUT32(SIO_GPIO_OUT_CLR,1<<25);

    //set the function select to SIO (software controlled I/O)
    PUT32(IO_BANK0_GPIO25_CTRL_RW,5);

    //output enable
    PUT32(SIO_GPIO_OE_SET,1<<25);
    while(1)
    {
        //turn on the led
        PUT32(SIO_GPIO_OUT_SET,1<<25);
        DELAY(0x100000);
        //turn off the led
        PUT32(SIO_GPIO_OUT_CLR,1<<25);
        DELAY(0x100000);
    }
    return(0);
}
