/* A demonstration routine for controlling MCP23017 chip outputs under C.

This routine sets up two chips, then turns the outputs by writing bytes.
After 5 seconds, it turns the outputs off.

You MUST run this program with root privileges - otherwise it won't work.
That's because we're writing directly to the hardware;
the Linux security policy is to avoid direct hardware access to regular users.
You must execute it with "sudo" (for example, "sudo demo").

This program depends on i2c-dev library, so you won't compile it without that.
*/


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

/*  Define constants for MCP23017 I2C bus addresses

  This address is set by pulling the pins A0, A1, A2 up (to +3.3V) or down (to GND).
  The major digit is 2, and it's set by manufacturer.
  The minor digit is an octal numbers of the byte from pins A2, A1, A0:

  We use two MCP23017 chips per Raspberry Monotype interface.

  addr  A2,A1,A0
  0x20  0, 0, 0
  0x21  0, 0, 1

  and further chips will have:
  0x22  0, 1, 0
  0x23  0, 1, 1
  0x24  1, 0, 0
  0x25  1, 0, 1
  0x26  1, 1, 0
  0x27  1, 1, 1
*/

#define MCP0_ADDR 0x20
#define MCP1_ADDR 0x21


// Define constants for MCP23017 register numbers. They'll be explained later.

#define IODIRA 0x00
#define IODIRB 0x01
#define GPIOA 0x12
#define GPIOB 0x13

// Define a constant for output and "low" status:
#define OUTPUT 0x00
#define ALL_OFF 0x00

static const char *device = "/dev/i2c-1";	// Filesystem path to access the I2C bus
uint8_t buffer[2];                              // Initialize a buffer for two bytes to write to the device
int mcp0, mcp1;



void all_off(void) {
  // This function sets all outputs on both chips to 0 (off, low state).

  // Chip 0
  buffer[0] = GPIOA;
  buffer[1] = ALL_OFF;
  write(mcp0, buffer, 2) ; //GPIOA to all off

  buffer[0] = GPIOB;
  buffer[1] = ALL_OFF;
  write(mcp0, buffer, 2) ; //GPIOB to all off

  // Chip 1
  buffer[0] = GPIOA;
  buffer[1] = ALL_OFF;
  write(mcp1, buffer, 2) ; //GPIOA to all off

  buffer[0] = GPIOB;
  buffer[1] = ALL_OFF;
  write(mcp1, buffer, 2) ; //GPIOB to all off
}



int mcp_init(void) {
// Chip initialization:
// The system (and kernel in particular) must set up communication
// with the MCP23017 chips over the I2C bus. It's done by opening
// the device file (/dev/i2c-1) and calling the "ioctl" function.

// The chip must also "know" that it's used for providing outputs.
// All the registers used are specified in the chip's datasheet:
// http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf

// Initialize 1st MCP23017 with 0x20 address:
mcp0 = open(device, O_RDWR);
ioctl(mcp0, I2C_SLAVE, MCP0_ADDR);

// Initialize 2nd MCP23017 with 0x21 address:
mcp1 = open(device, O_RDWR);
ioctl(mcp1, I2C_SLAVE, MCP1_ADDR);



/*Writing bytes to registers:


  Each MCP23017 has two input/output banks, and we can read or write
  bytes to/from them at once.

  Bank A (pins 21...28, named GPA0...GPA7)
  Bank B (pins 1...8, named GPB0...GPB7)


  The MCP23017 has many registers we can write bytes from, but the ones
  that interest us are (with their hex values):

  IODIRA - 0x00  - for setting the direction (input or output) for the bank; 0 - output, 1 - input
  IODIRB - 0x01  - as above for bank B
  GPIOA  - 0x12  - for writing the data to outputs or reading the inputs of bank A
  GPIOB  - 0x13  - as above for bank B

  First, we need to set the IODIRA and IODIRB registers which control
  the input or output direction for all GPIO pins of bank A or B.

  Write 0x00 for all outputs, 0xff for all inputs, or any other hex number
  if you want some pins to be outputs and other to be inputs.

  */


  // First, we must set the I/O direction to output.
  // Write 0x00 to all IODIRA and IODIRB registers.

  // Chip 0
  buffer[0] = IODIRA;
  buffer[1] = OUTPUT;
  write(mcp0, buffer, 2) ;  // set IODIRA to all outputs

  buffer[0] = IODIRB;
  buffer[1] = OUTPUT;
  write(mcp0, buffer, 2) ;  // set IODIRB to all outputs

  // Chip 1
  buffer[0] = IODIRA;
  buffer[1] = OUTPUT;
  write(mcp1, buffer, 2) ;  // set IODIRA to all outputs

  buffer[0] = IODIRB;
  buffer[1] = OUTPUT;
  write(mcp1, buffer, 2) ;  // set IODIRB to all outputs


  // Now we should initially set the outputs' state to low

  // Do the same whenever you want to turn all outputs off at once.
  // It should be easier to do this with a "for" loop (DRY principle).

all_off();

}


void send_bytes(int byte1, int byte2, int byte3, int byte4) {
  // This function sends bytes (received as arguments) 
  // to the chips' GPIOA, GPIOB registers.

  // Chip 0
  buffer[0] = GPIOA;
  buffer[1] = byte1;
  write(mcp0, buffer, 2) ; //GPIOA set byte 1

  buffer[0] = GPIOB;
  buffer[1] = byte2;
  write(mcp0, buffer, 2) ; //GPIOB set byte 2

  // Chip 1
  buffer[0] = GPIOA;
  buffer[1] = byte3;
  write(mcp1, buffer, 2) ; //GPIOA set byte 3

  buffer[0] = GPIOB;
  buffer[1] = byte4;
  write(mcp1, buffer, 2) ; //GPIOB set byte 4
}




int main(void) {

  // Main loop: 

  // Initialize the chips:
  mcp_init();

  // Send four bytes to the chips:

  // Call a function send_bytes(byte0, byte1, byte2, byte3).
  // You can send them as hex, decimal, octal or bin values.
  send_bytes(0x11, 53, 0144, 0b10101010);

  // Wait 5s so that you see the outputs turned on:
  sleep(5) ;

  // Now turn all the outputs off - write 0x00 to GPIOA, GPIOB
  // on both chips:
  all_off();

  // This will be the end of our demonstration program.
  return 0 ;
}
