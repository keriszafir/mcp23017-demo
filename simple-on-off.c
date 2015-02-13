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


static const char *device = "/dev/i2c-1";	// Fliesystem path to access the I2C bus

int main() {

// Initialize 1st MCP23017 with 0x20 address:
int mcp0;
mcp0 = open(device, O_RDWR);
ioctl(mcp0, I2C_SLAVE, 0x20);

// Initialize 2nd MCP23017 with 0x21 address:
int mcp1;
mcp1 = open(device, O_RDWR);
ioctl(mcp1, I2C_SLAVE, 0x21);

/*Writing bytes to registers:

  We use a function wiringPiI2CWriteReg8(device, int reg, int data)

  The first number is an I2C address of a MCP23017 chip.
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
  uint8_t buffer[2];

  // First, we must set the I/O direction to output.
  // Write 0x00 to all IODIRA and IODIRB registers.

  // Chip 0
  buffer[0] = 0x00;
  buffer[1] = 0x01;
  write(mcp0, buffer, 2) ;  // set IODIRA to 0x00, all outputs

  buffer[0] = 0x01;
  buffer[1] = 0x01;
  write(mcp0, buffer, 2) ;  // set IODIRB to 0x00, all outputs

  // Chip 1
  buffer[0] = 0x00;
  buffer[1] = 0x01;
  write(mcp1, buffer, 2) ;  // set IODIRA to 0x00, all outputs

  buffer[0] = 0x01;
  buffer[1] = 0x01;
  write(mcp1, buffer, 2) ;  // set IODIRB to 0x00, all outputs

  // Now we should initially set the outputs' state to low
  // by writing 0x00 byte to GPIOA and GPIOB.
  // Do the same whenever you want to turn all outputs off at once.
  // It should be easier to do this with a "for" loop (DRY principle),
  // and define a function that can be called from your program.

  // Chip 0
  buffer[0] = 0x12;
  buffer[1] = 0x00;
  write(mcp0, buffer, 2) ; //GPIOA to 0x00, all off

  buffer[0] = 0x13;
  buffer[1] = 0x00;
  write(mcp0, buffer, 2) ; //GPIOB to 0x00, all off

  // Chip 1
  buffer[0] = 0x12;
  buffer[1] = 0x00;
  write(mcp1, buffer, 2) ; //GPIOA to 0x00, all off

  buffer[0] = 0x13;
  buffer[1] = 0x00;
  write(mcp1, buffer, 2) ; //GPIOB to 0x00, all off


  // We're slowly getting to turn the outputs on...
  // Let's set up some arbitrary bytes to send.

  int byte1 ;
  int byte2 ;
  int byte3 ;
  int byte4 ;

  byte1 = 0xaa ;
  byte2 = 0x40 ;
  byte3 = 0x15 ;
  byte4 = 0xec ;

  // Now, let's write these bytes to the outputs!
  // Chip 0
  buffer[0] = 0x12;
  buffer[1] = byte1;
  write(mcp0, buffer, 2) ; //GPIOA set byte 1

  buffer[0] = 0x13;
  buffer[1] = byte2;
  write(mcp0, buffer, 2) ; //GPIOB set byte 2

  // Chip 1
  buffer[0] = 0x12;
  buffer[1] = byte3;
  write(mcp1, buffer, 2) ; //GPIOA set byte 3

  buffer[0] = 0x13;
  buffer[1] = byte4;
  write(mcp1, buffer, 2) ; //GPIOB set byte 4


  // Wait 5s so that you see the outputs turned on:
  sleep(5) ;

  // Now turn all the outputs off - write 0x00 to GPIOA, GPIOB
  // on both chips:

  // Chip 0
  buffer[0] = 0x12;
  buffer[1] = 0x00;
  write(mcp0, buffer, 2) ; //GPIOA to 0x00, all off

  buffer[0] = 0x13;
  buffer[1] = 0x00;
  write(mcp0, buffer, 2) ; //GPIOB to 0x00, all off

  // Chip 1
  buffer[0] = 0x12;
  buffer[1] = 0x00;
  write(mcp1, buffer, 2) ; //GPIOA to 0x00, all off

  buffer[0] = 0x13;
  buffer[1] = 0x00;
  write(mcp1, buffer, 2) ; //GPIOB to 0x00, all off

  // This will be the end of our demonstration program.

  return 0 ;
}
