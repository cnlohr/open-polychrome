// To compile
//   tcc openpolychrome.c -run -lsetupapi

#include <stdio.h>
#include <windows.h>
#include "hidapi.h"
#include "hidapi.c"

// Technique
//  Install Wireshark + USBMon
//  Monitor Bus
//  Start polychrome
//  Right-click on all things that are flying by when not changnig color
//    Filter->And as not...
//  Start changing colors make sure you get those USB packets (Maybe you're on the wrong bus)
//  Look at what's left when you have it.
//  Scroll up to GET DESCRIPTOR Response Device
//  Identify device VID/PID.
//  Look at RGB values find the bits that are the actual colors.
//  Shove some bits down the pipe and see if it looks similar.

int main()
{
	hid_init();
	hid_device * dev = hid_open( 0x26ce, 0x01a2, 0 );
	
	uint8_t data[64] = { 0 };
	int i = 0;
	while( 1 )
	{
		data[0] = 0; // size or type or something
		data[1] = 0x10;
		data[2] = 0;
		data[3] = 6;
		data[4] = 1;
		data[5] = i;
		data[6] = i;
		data[7] = i;
		data[8] = 0x80;
		data[9] = 0xff;
		i+=128;
		int r = HID_API_CALL hid_write( dev, data, 64 );
		printf( "R: %d\n", r); 
		Sleep( 10 );
	}
}
