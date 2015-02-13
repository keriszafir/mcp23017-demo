/* MCP23017 control program for demonstration purposes

This program uses a system call to the i2cset program from i2c-tools package.
It initializes two MCP23017 chips and sends bytes to registers.
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
char command[50];
sprintf(command, "i2cset -y 1 %i %i %i", chip_address, register_address, value);
system(command);
}

void all_off(void) {
//All off: write 0x00 to all outputs
write_data(MCP0, GPIOA, 0x00);
write_data(MCP0, GPIOB, 0x00);
write_data(MCP1, GPIOA, 0x00);
write_data(MCP1, GPIOB, 0x00);
}

void init(void) {
//Init: set all pins as outputs and set them to 0
write_data(MCP0, IODIRA, 0x00);
write_data(MCP0, IODIRB, 0x00);
write_data(MCP1, IODIRA, 0x00);
write_data(MCP1, IODIRB, 0x00);
all_off();
}

void send_bytes(int byte0, int byte1, int byte2, int byte3) {
//Send specified bytes to registers
write_data(MCP0, GPIOA, byte0);
write_data(MCP0, GPIOB, byte1);
write_data(MCP1, GPIOA, byte2);
write_data(MCP1, GPIOB, byte3);
}

void main(void) {
//Set some arbitrary bytes
int byte0;
byte0 = 0x11;
int byte1;
byte1 = 0x22;
int byte2;
byte2 = 0x33;
int byte3;
byte3 = 0x44;

//Initialize chips
init();
send_bytes(byte0, byte1, byte2, byte3);
sleep(5);
all_off();
}
