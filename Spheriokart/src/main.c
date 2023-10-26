#include "em_usb.h"
#include "trackball.h"
#include "segmentlcd.h" // For writing to the devboard screen. Can be removed when we are running on our own board.
#include "kart.h"
#include "printing.h"
#include "spi.h"

int main(){
	TrackballValues v;
	SWO_SetupForPrint(); /* For adding printing to console in simplicity studio debugger */

	spi_init();

	for(;;){ // Keep trying to connect

		InitUSBHStack(); /* Initialize usb stack */
		InitTrackball(); /* Initialize the trackball */
		while(USBH_DeviceConnected()){ /* Program loop */
			GetTrackballValues(&v);
			kart(v);
		}

		/* USB connection lost */
		SegmentLCD_NumberOff();
		SegmentLCD_Write("Device");
		USBTIMER_DelayMs(500);
		SegmentLCD_Write("Removed");
		USBTIMER_DelayMs(500);
		SegmentLCD_Write("USBHOST");

	}
}
