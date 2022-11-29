echo "Compiling the program..."
avr-gcc -w -Os -DF_CPU=8000000UL -mmcu=atmega32 -fexec-charset=CP866 -lgcc *.c I2C/*.c SSD1306/*.c -o main

echo "Generating .hex file..."
avr-objcopy -O ihex -R .eeprom main main.hex

echo "Removing junk..."
rm main

echo "Program is in file 'main.hex'!"
