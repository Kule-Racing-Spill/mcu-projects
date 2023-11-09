#include "em_usb.h"
#include "trackball.h"
#include "segmentlcd.h" // For writing to the devboard screen. Can be removed when we are running on our own board.
#include "kart.h"
#include "printing.h"
#include "spi.h"
#include "em_device.h"
#include "button.h"

int main(){
	TrackballValues v;
	SWO_SetupForPrint(); /* For adding printing to console in simplicity studio debugger */

	spi_init(); /* SPI init */

	/* Button stuff */
	Button_Init(BUTTON1|BUTTON2);
	uint32_t b;

	spi_send_sprites();

	for(;;){ // Keep trying to connect

		InitUSBHStack(); /* Initialize usb stack */
		InitTrackball(); /* Initialize the trackball */

		while(USBH_DeviceConnected()){ /* Program loop */
			GetTrackballValues(&v);
			b = Button_ReadReleased();
			if( b&BUTTON1 ) {
				printf("Button1 pressed!\n");
			}
			if( b&BUTTON2 ) {
				printf("Button2 pressed!\n");
			}
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
