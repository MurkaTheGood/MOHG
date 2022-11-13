echo "Compiling the program..."
# avr-gcc -w -Os -DF_CPU=8000000UL -mmcu=atmega32 -fexec-charset=CP866 -c -o main.o main.c
# avr-gcc -w -Os -DF_CPU=8000000UL -mmcu=atmega32 -fexec-charset=CP866 -c -o I2C.o I2C/I2C.c
# avr-gcc -w -Os -DF_CPU=8000000UL -mmcu=atmega32 -fexec-charset=CP866 -c -o SSD1306.o SSD1306/SSD1306.c

avr-gcc -w -Os -DF_CPU=8000000UL -mmcu=atmega32 -fexec-charset=CP866 -lgcc *.c I2C/*.c SSD1306/*.c -o main

# echo "Linking the program..."
# avr-gcc -w -lgcc -mmcu=atmega32 main.o I2C.o SSD1306.o -o main

echo "Generating .hex file..."
avr-objcopy -O ihex -R .eeprom main main.hex

echo "Removing junk..."
# rm main.o
# rm I2C.o
# rm SSD1306.o
rm main

echo "Program is in file 'main.hex'!"
