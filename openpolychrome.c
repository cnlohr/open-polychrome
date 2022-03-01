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

/// And then you find out someone's already RE'd it.
// https://gitlab.com/CalcProgrammer1/OpenRGB/-/blob/master/Controllers/ASRockPolychromeUSBController/ASRockPolychromeUSBController.cpp

int main()
{
	hid_init();
	hid_device * dev = hid_open( 0x26ce, 0x01a2, 0 );
	printf( "Device: %p\n", dev );
	uint8_t data[65] = { 0 };
	int i = 0;
	while( 1 )
	{
		uint8_t * dptr = data;
		*(dptr++) = 0; // Just hidapi being hidapi
		*(dptr++) = 0x10;
		*(dptr++) = 0;
		*(dptr++) = 6; // zone (Board backside = 6, this can be changed)
		*(dptr++) = 1; // mode
		*(dptr++) = (i&1)?255:0;
		*(dptr++) = (i&1)?0:255;
		*(dptr++) = (i&1)?255:0;
		*(dptr++) = 0x80;
		*(dptr++) = 0xff; //Intensity
		*(dptr++) = 0; //Allzone?
		i++;
		int r = HID_API_CALL hid_write( dev, data, 65 );
		printf( "R: %d / %d\n", r, i ); 
		Sleep( 20 );
	}
}
