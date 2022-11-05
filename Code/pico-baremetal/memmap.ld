MEMORY
{
    flash : ORIGIN = 0x10000000, LENGTH = 0xFC
}
SECTIONS
{
    .text   : { *(.text*)   } > flash
}

