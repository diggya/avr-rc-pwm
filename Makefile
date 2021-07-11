
all:
	echo Define action, eeprom-read, eeprom-write, flash
fuses:
	sudo avrdude -c usbasp -p t85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
eeprom-read:
	sudo avrdude -c usbasp -p t85 -U eeprom:r:eeprom.raw:r
eeprom-write:
	sudo avrdude -c usbasp -p t85 -U eeprom:w:eeprom.sample:r
flash: main.hex
	sudo avrdude -c usbasp -p t85 -U flash:w:main.hex
%.o: %.c
	avr-gcc -g -Os -mmcu=attiny85 -c $< -o $@
%.elf: %.o
	avr-gcc -g -mmcu=attiny85 -o $@ $<
%.lst: %.elf
	avr-objdump -h -S $< > $@
%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
