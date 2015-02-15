/*
  A demonstration routine for controlling MCP23017 chip outputs under C.

  This routine sets up two chips, then turns the outputs by writing bytes.
  After 5 seconds, it turns the outputs off.

  You MUST run this program with root privileges - otherwise it won't work.
  That's because we're writing directly to the hardware;
  the Linux security policy is to avoid direct hardware access to regular users.
  You must execute it with "sudo" (for example, "sudo demo").

  This program depends on i2c-dev library, so you won't compile it without that.
  It also uses wiringPi for handling interrupts. wiringPi is available at:
  http://wiringpi.com

  You MUST compile it with a command:

  gcc input_file.c -o output_file -lwiringPi
*/

#include <wiringPi.h>
#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

/*
  Define constants for MCP23017 I2C bus addresses

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

// Define the physical pin number for the input we'll handle the interrupts from:
# define INPUT_NO 11

// Define I2C addresses for MCP23017 chips:
#define MCP0_ADDR 0x20
#define MCP1_ADDR 0x21

// Define constants for MCP23017 register numbers. They'll be explained later.
#define IODIRA 0x00
#define IODIRB 0x01
#define GPIOA 0x12
#define GPIOB 0x13

// Define a constant for output and "low" status:
#define OUTPUT_BYTE 0x00
#define ALL_OFF 0x00

// Declare some global variables:
static const char *device = "/dev/i2c-1";       // Filesystem path to access the I2C bus
uint8_t buffer[2];                              // Buffer for two bytes to write to the device
int mcp0, mcp1;                                 // Both MCP23017 chip file descriptors
static volatile int interrupt, last_state;      // For storing the input's state
struct timeval last_change;                     // For storing the last state change




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
/*
  Chip initialization:
  The system (and kernel in particular) must set up communication
  with the MCP23017 chips over the I2C bus. It's done by opening
  the device file (/dev/i2c-1) and calling the "ioctl" function.

  The chip must also "know" that it's used for providing outputs.
  All the registers used are specified in the chip's datasheet:
  http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf
*/

// Initialize 1st MCP23017 with 0x20 address:
mcp0 = open(device, O_RDWR);
ioctl(mcp0, I2C_SLAVE, MCP0_ADDR);

// Initialize 2nd MCP23017 with 0x21 address:
mcp1 = open(device, O_RDWR);
ioctl(mcp1, I2C_SLAVE, MCP1_ADDR);


/*
  Writing bytes to registers:

  Each MCP23017 has two input/output banks, and we can read or write
  bytes to/from them at once.

  Bank A (pins 21...28, named GPA0...GPA7)
  Bank B (pins 1...8, named GPB0...GPB7)

  The MCP23017 has many registers we can write bytes from, but the ones
  that interest us are (with their hex addresses):

  IODIRA - 0x00  - for setting the direction (0 - in, 1 - out) for the bank
  IODIRB - 0x01  - as above for bank B
  GPIOA  - 0x12  - for writing to outputs or reading the inputs of bank A
  GPIOB  - 0x13  - as above for bank B

  First, we need to set the IODIRA and IODIRB registers which control
  the input or output direction for all GPIO pins of bank A or B.

  Write 0x00 for all outputs, 0xff for all inputs, or any other hex number
  if you want some pins to be outputs and other to be inputs.

  First, we must set the I/O direction to output.
  Write 0x00 to all IODIRA and IODIRB registers.
*/
// Chip 0
  buffer[0] = IODIRA;
  buffer[1] = OUTPUT_BYTE;
  write(mcp0, buffer, 2) ;  // set IODIRA to all outputs

  buffer[0] = IODIRB;
  buffer[1] = OUTPUT_BYTE;
  write(mcp0, buffer, 2) ;  // set IODIRB to all outputs

// Chip 1
  buffer[0] = IODIRA;
  buffer[1] = OUTPUT_BYTE;
  write(mcp1, buffer, 2) ;  // set IODIRA to all outputs

  buffer[0] = IODIRB;
  buffer[1] = OUTPUT_BYTE;
  write(mcp1, buffer, 2) ;  // set IODIRB to all outputs


// Now we should initially set the outputs' state to low.
all_off();
}


void set_outputs(int byte1, int byte2, int byte3, int byte4) {
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


void interrupt_handler(void) {
/*
  Here all the magic happens...
  Upon interrupt, this function sets the "interrupt" variable to 1.
  This variable is polled in the send_codes function, which catches
  the interrupt, resets the variable to 0 and checks the input status.
*/
  struct timeval now;
  unsigned long diff;

  gettimeofday(&now, NULL);

// Time difference - set 10ms (1/100s)
  diff = (now.tv_sec * 1000000 + 20000) - (last_change.tv_sec * 1000000 + last_change.tv_usec);

// Filter any changes in intervals shorter than diff
// (like contact bouncing etc.).
// Change the status:
  if (diff > 10000) {
    interrupt = 1;
  }

// Store the time for last state change:
  last_change = now;
}


void interface_setup(void) {
// Setup function: initialize all the inputs and outputs first:

// Initialize the chips:
  mcp_init();

// Initialize the interrupt handling by wiringPi:
  wiringPiSetupPhys();
  pinMode(INPUT_NO, OUTPUT);
  wiringPiISR(INPUT_NO, INT_EDGE_BOTH, &interrupt_handler);
// Initialize the last_change timer
  gettimeofday(&last_change, NULL);
}


void send_codes(int byte0, int byte1, int byte2, int byte3) {
/*
  Wait until the interrupt, then check the input state and send codes.

  Reset interrupt status just in case there was an interrupt before
  we called the function. This will prevent erroneous sending
  of signals when there's no actual on-off cycle on input.
*/
  interrupt = 0;

// Set the busy condition to 1:
  int interface_busy;
  interface_busy = 1;

// Hold execution for an entire on-off cycle:
// check if the interface is still busy - if so, run the loop
  while (interface_busy) {

//  Wait and do nothing until we catch the interrupt
    for (;!interrupt;) {
    sleep(0.1);
    }
    interrupt = 0;           // Reset the interrupt state

/*
    Check it the input went on or off (wiringPi can't discriminate
    between interrupts on rising or falling edge - if we set them
    to INT_EDGE_BOTH, the same event will be generated when the input
    is turned on or off. We have to set a last_state variable
    in software, and check its value each time an interrupt is generated.
*/
    if (digitalRead(INPUT_NO)) {   // On turning the input on
      set_outputs(byte0, byte1, byte2, byte3);
    }
    else {                    // On turning the input off
      all_off();              // Turn all outputs off
      interface_busy = 0;     // Release the interface
    }
//    last_state =! last_state;  // Toggle the last state
  }
}

