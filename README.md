# mcp23017-demo
Demonstration programs for controlling MCP23017 I/O expanders

These programs will help you understand how to control MCP23017 expanders
(most notably, setting them up as outputs).

You must have the libi2c-dev installed.

Normally, Linux security policy prohibits direct access to hardware for non-root users.
You can change it by creating a file by creating a udev rule file:
```
sudo nano /etc/udev/rules.d/60-i2c-tools.rules
```
with contents as following:
```
KERNEL=="i2c-[1-9]*", GROUP="i2c", MODE="0660"
```
This will allow anyone from i2c group to write to /dev/i2c-*.

You will have to add your user account to the i2c group:
```
sudo adduser [username] i2c
``` 

The udev rule file is included for convenience, you can copy it into /etc/udev/rules.d

It's best to compile and run this on a Raspberry Pi mod B revision 2, or newer.
A+, B+ and RPi 2 will work too.
Any Linux distribution should work, although Raspbian wheezy is the most popular
and you want to stick with this.
