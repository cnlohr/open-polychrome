// To compile
//   tcc openpolychrome.c -run -lsetupapi -lws2_32

#include <stdio.h>
#include <windows.h>
#include "hidapi.h"
#include "hidapi.c"

#define MINIOSC_IMPLEMENTATION
#include "miniosc.h"

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

hid_device * dev;

void oscCallback( const char * address, const char * type, void ** parameters )
{
	if( strcmp( address, "/opc/zone6" ) == 0 )
	{
		if( strcmp( type, ",r" ) == 0 )
		{
			uint32_t colorval = *((uint32_t*)parameters[0]);
			uint8_t data[65] = { 0 };
			uint8_t * dptr = data;
			*(dptr++) = 0; // Just hidapi being hidapi
			*(dptr++) = 0x10;
			*(dptr++) = 0;
			*(dptr++) = 6; // zone (Board backside = 6, this can be changed)
			*(dptr++) = 1; // mode
			*(dptr++) = (colorval>>0)&0xff;
			*(dptr++) = (colorval>>8)&0xff;
			*(dptr++) = (colorval>>16)&0xff;
			*(dptr++) = 0x00;
			*(dptr++) = 0xff; //Intensity
			*(dptr++) = 0; //Allzone?
			int r = hid_write( dev, data, 65 );
			hid_read( dev, data, 64 );
			printf( "R: %d [%08x]\n", r, colorval );
		}
		else
		{
			printf( "Unknown parmaeters: %s\n", type );
		}
	}
	else
	{
		printf( "Unknown message: %s\n", address );
	}
}

int main()
{
	hid_init();
	dev = hid_open( 0x26ce, 0x01a2, 0 );
	printf( "Device: %p\n", dev );
	if( !dev )
	{
		fprintf( stderr, "Error: No Polychrome USB Motherboard detected.\n" );
		return -5;
	}
	
	int minioscerrorcode = 0;
	miniosc * osc = minioscInit( 9993, 0, 0, &minioscerrorcode );
	if( !osc )
	{
		fprintf( stderr, "Error: could not initialize miniosc. Error code: %d\n", minioscerrorcode );
		return -6;
	}
	
	while(1)
	{
		minioscPoll( osc, 10000, oscCallback );
	}
	
}
