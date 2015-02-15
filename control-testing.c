#include "./interface.c"

void main(void) {
/*
  In the main loop of your program, you need to
  call the interface_setup() function to get it
  to work:
*/
  interface_setup();
/*
  Then, you just call the send_codes()
  function with a set of four bytes, no matter
  whether you give the hex, bin, decimal or
  octal numbers.

  The send_codes function runs for an entire
  cycle, then exits and your program moves on
  to the next send_codes call.
*/
  send_codes(0x11, 53, 0144, 0b10101010);
  send_codes(0x22, 42, 0265, 0b01010101);
  send_codes(0x11, 53, 0144, 0b10101010);
  send_codes(0x22, 42, 0265, 0b01010101);
  send_codes(0x11, 53, 0144, 0b10101010);
  send_codes(0x22, 42, 0265, 0b01010101);
  send_codes(0x11, 53, 0144, 0b10101010);
  send_codes(0x22, 42, 0265, 0b01010101);
  send_codes(0x11, 53, 0144, 0b10101010);
  send_codes(0x22, 42, 0265, 0b01010101);
  send_codes(0x11, 53, 0144, 0b10101010);
  send_codes(0x22, 42, 0265, 0b01010101);
}
