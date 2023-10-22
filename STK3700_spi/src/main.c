#include "spidrv.h"
#include "segmentlcd.h"

void TransferComplete( SPIDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       int itemsTransferred )
{
	if ( transferStatus == ECODE_EMDRV_SPIDRV_OK )
	{
	// Success !
		SegmentLCD_Number(2);
	}
}


SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

int main( void )
{
	SegmentLCD_Init(false);
	SegmentLCD_Number(0);

	uint8_t buffer[10];
	SPIDRV_Init_t initData = SPIDRV_MASTER_USART2;

	// Initialize a SPI driver instance
	SPIDRV_Init( handle, &initData );

	// Transmit data using a blocking transmit function
	SPIDRV_MTransmitB( handle, buffer, 10 );
	SegmentLCD_Number(1);

	// Transmit data using a callback to catch transfer completion.
	SPIDRV_MTransmit( handle, buffer, 10, TransferComplete );
}
