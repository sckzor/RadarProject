ARMGNU ?= arm-none-eabi

XCPU = -mcpu=cortex-m0
AOPS = --warn --fatal-warnings $(XCPU)
COPS = -Wall -O2 -ffreestanding -nostartfiles $(XCPU)
LOPS = -nostdlib 

all: main.bin

bootstrap.o: bootstrap.s
	$(ARMGNU)-as $(AOPS) bootstrap.s -o bootstrap.o

main.o: main.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c main.c -o main.o

main.bin: memmap.ld bootstrap.o main.o
	$(ARMGNU)-ld $(LOPS) -T memmap.ld bootstrap.o main.o -o main.elf
	$(ARMGNU)-objdump -D main.elf > main.list
	$(ARMGNU)-objcopy -O binary main.elf main.bin

flash: main.bin
	makeuf2/makeuf2 main.bin main.uf2
	sudo picotool load main.uf2

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.uf2

