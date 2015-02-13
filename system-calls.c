/* MCP23017 control program for demonstration purposes

This program uses a system call to the i2cset program from i2c-tools package.
It initializes two MCP23017 chips and sends bytes to registers.

This program uses the i2cset utility from the i2c-tools package. It is recommended
to set up the I2C write access for regular users as described in README.md,
and run the program under a regular user account (add yourself to the i2c group before).

The MCP23017 chip is an I2C to 16 GPIO expander. It provides 16 input/output ports,
grouped in two banks: A and B. It has a bunch of registers you can write bytes to;
you can find them in a datasheet at http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf

What interests us is controlling the outputs by writing bytes to GPIOA and GPIOB registers.
The method will be described in this file.

Any Raspberry Pi more recent than the old 256MB B rev 1 (i.e., B rev 2, A+, B+ and RPi 2 mod A, B)
uses I2C bus 1 for interfacing with devices by GPIO.
Earlier models - RPi B rev 1 (and probably some RPi A's too) - used I2C bus 0.
This program assumes you have B rev 2 or newer; you can change this by setting
the bus_id value to 0 in the write_data function.

You can probe for I2C devices on the bus 1 by typing a command:
i2cdetect -y 1
(you must have access rights for the I2C bus, see README.md)

You'll probably get an output like this:

     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: 20 21 -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --   

This was obtained on a Raspberry mod B rev 2 with two MCP23017 chips,
they have addresses 0x20 (or 32), and 0x21 (or 33).

As for setting these addresses...

MCP23017 has three pins: A2, A1, A0 - which are used for setting its I2C address.
If you use multiple chips, each of them MUST have a unique I2C address; you change it
by pulling the A2, A1, A0 pins up (to 3.3V) or down (to GND).

Notice that the last digit of an address is an octal number for a combination of bits: 

A2  A1  A0   number  I2C address
0   0   0      0        0x20
0   0   1      1        0x21
0   1   0      2        0x22
0   1   1      3        0x23
1   0   0      4        0x24
1   0   1      5        0x25
1   1   0      6        0x26
1   1   1      7        0x27

You can use up to 8 MCP23017 chips on a single I2C bus,
and this would provide 128 additional input/outputs.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MCP0 0x20    // I2C address for chip 0
#define MCP1 0x21    // I2C address for chip 1
#define IODIRA 0x00  // hex address for IODIRA register - bank A direction (0 - out, 1 - in)
#define IODIRB 0x01  // hex address for IODIRB register - bank B direction (0 - out, 1 - in)
#define GPIOA 0x12   // hex address for GPIOA register - bank A input/outputs
#define GPIOB 0x13   // hex address for GPIOB register - bank B input/outputs

void write_data(int chip_address, int register_address, int value) {
/*Wrapper function for "system" call to "i2cset" utility.
Arguments:
chip_address     - MCP23017 I2C bus address (0x20, 0x21)
register_address - hex or decimal int for register address, it's convenient to use constants as above
value            - byte we want to write to the register (hex, decimal, bin)
*/
int bus_id;
bus_id = 1                // change this to 0 if you use an old RPi mod B rev 1, or early mod A
char command[50];
sprintf(command, "i2cset -y %i %i %i %i", bus_id, chip_address, register_address, value);
system(command);
}

void all_off(void) {
//All off: write 0x00 to all outputs to turn them off
write_data(MCP0, GPIOA, 0x00);
write_data(MCP0, GPIOB, 0x00);
write_data(MCP1, GPIOA, 0x00);
write_data(MCP1, GPIOB, 0x00);
}

void init(void) {
//Initialization: set all pins as outputs and set them to 0
write_data(MCP0, IODIRA, 0x00);
write_data(MCP0, IODIRB, 0x00);
write_data(MCP1, IODIRA, 0x00);
write_data(MCP1, IODIRB, 0x00);
all_off();
}

void send_bytes(int byte0, int byte1, int byte2, int byte3) {
//A function that send specified bytes to registers, all at once
write_data(MCP0, GPIOA, byte0);
write_data(MCP0, GPIOB, byte1);
write_data(MCP1, GPIOA, byte2);
write_data(MCP1, GPIOB, byte3);
}

void main(void) {
//Set some arbitrary bytes that we'll send to outputs
int byte0;
byte0 = 0x11;       // hex
int byte1;
byte1 = 0177;       // octal
int byte2;
byte2 = 51;
int byte3;
byte3 = 0b11001100; // binary - it'll work if you compile it with gcc

//Initialize chips
init();

//Send these bytes!
send_bytes(byte0, byte1, byte2, byte3);
//Wait 5 seconds
sleep(5);
//Turn all lines off
all_off();
}
