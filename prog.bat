SET project=gas_Sensor
SET chip=attiny13

rm %project%.hex 
cls

avr-gcc -g -Os -mmcu=%chip% -c %project%.c

avr-gcc -g -mmcu=%chip% -o %project%.elf %project%.o

avr-objcopy -j .text -j .data -O ihex %project%.elf %project%.hex

rem Programming/Flas istructions with usbtiny
rem sudo avrdude -c usbtiny -p %chip% -U flash:w:%project%.hex

rm %project%.elf 
rm %project%.o


