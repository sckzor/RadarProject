
.cpu cortex-m0
.thumb

    ldr r0,=0x20001000
    mov sp,r0
    bl blink
    b .
