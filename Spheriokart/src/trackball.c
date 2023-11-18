#include "em_usb.h"
// #include "segmentlcd.h"
#include "trackball.h"

/* Timer indices. */
#define KBD_POLL_TIMER 1
#define KBD_SUSPEND_TIMER 2

#define KBD_USB_ADDRESS 1   /* USB address used for trackball. 			*/
#define KBD_INT_IN_HC 2     /* Host channel nr. used for trackball interrupt IN endpoint. Ch. 0 and 1 defaults to the control endpoint (EP0) */
#define PAD_IN_REPORT_LEN 4 /* Length of trackball interrupt IN report.	*/

/* Static functions */
static int HidSetIdle(uint8_t duration, uint8_t reportId);
static int HidSetProtocol(void);
static void PollTimeout();
static void SuspendTimeout();
static void CheckPadSwipe(vec2int *coords);

/* Static globals */
static int8_t usbData[8];          /* Needs to be signed integer for reading negative inputs from trackball */
static USBH_Device_TypeDef device; /* USB Host device definition */
static USBH_Ep_TypeDef ep;         /* USB Host endpoint definition */
static volatile bool pollTimerDone;
static volatile bool suspendTimerDone;

STATIC_UBUF(tmpBuf, 1024);

void InitUSBHStack(void)
{
    USBH_Init_TypeDef is = USBH_INIT_DEFAULT; /* USB Host stack initialization structure */
    USBH_Init(&is);                           /* Initialize USB HOST stack */
}

void GetTrackballValues(vec2int *coords)
{
    if (pollTimerDone)
    {
        CheckPadSwipe(coords);
        pollTimerDone = false;
        USBTIMER_Start(KBD_POLL_TIMER, ep.epDesc.bInterval, PollTimeout);
    }
}

void InitTrackball(void)
{
    // SegmentLCD_Init(false);
    int connectionResult = USBH_WaitForDeviceConnectionB(tmpBuf, 10);
    if (connectionResult == USB_STATUS_OK)
    {
        // SegmentLCD_Write("Device");
        USBTIMER_DelayMs(500);
        // SegmentLCD_Write("Added");
        USBTIMER_DelayMs(500);
        if (USBH_QueryDeviceB(tmpBuf, sizeof(tmpBuf), USBH_GetPortSpeed()) == USB_STATUS_OK)
        {
            USBH_InitDeviceData(&device, tmpBuf, &ep, 1, USBH_GetPortSpeed()); /* Initialize device data structure, assume device has 1 endpoint. */
            USBH_SetAddressB(&device, KBD_USB_ADDRESS);
            USBH_SetConfigurationB(&device, device.confDesc.bConfigurationValue);
            USBH_AssignHostChannel(&ep, KBD_INT_IN_HC); /* Assign a Host Channel to the interrupt IN endpoint. */
            HidSetIdle(0, 0);
            HidSetProtocol();
        }
        pollTimerDone = false;
        suspendTimerDone = false;
        USBTIMER_Start(KBD_POLL_TIMER, ep.epDesc.bInterval, PollTimeout);
        USBTIMER_Start(KBD_SUSPEND_TIMER, 5000, SuspendTimeout);
    }
    else if (connectionResult == USB_STATUS_TIMEOUT)
    {
        /* Connection timed out */
        // SegmentLCD_Write("TIMEOUT");
    }
}

/**************************************************************************/ /**
                                                                              * @brief
                                                                              *   Send HID class USB_HID_SET_IDLE setup command to the trackball.
                                                                              *
                                                                              * @param[in] duration  How often the trackball shall report status changes.
                                                                              * @param[in] reportId  The ID of the status change report.
                                                                              *
                                                                              * @return USB_STATUS_OK, or an appropriate USB transfer error code.
                                                                              *****************************************************************************/
static int HidSetIdle(uint8_t duration, uint8_t reportId)
{
    return USBH_ControlMsgB(
        &device.ep0,
        USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_INTERFACE |
            USB_SETUP_TYPE_CLASS_MASK, /* bmRequestType */
        USB_HID_SET_IDLE,              /* bRequest      */
        (duration << 8) | reportId,    /* wValue        */
        0,                             /* wIndex        */
        0,                             /* wLength       */
        NULL,                          /* void* data    */
        1000);                         /* int timeout   */
}

/**************************************************************************/ /**
                                                                              * @brief
                                                                              *   Send HID class USB_HID_SET_PROTOCOL setup command to the trackball.
                                                                              *
                                                                              * @return USB_STATUS_OK, or an appropriate USB transfer error code.
                                                                              *****************************************************************************/
static int HidSetProtocol(void)
{
    return USBH_ControlMsgB(
        &device.ep0,
        USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_INTERFACE |
            USB_SETUP_TYPE_CLASS_MASK, /* bmRequestType */
        USB_HID_SET_PROTOCOL,          /* bRequest      */
        0,                             /* wValue 0=boot protocol */
        0,                             /* wIndex        */
        0,                             /* wLength       */
        NULL,                          /* void* data    */
        1000);                         /* int timeout   */
}

/**************************************************************************/ /**
                                                                              * @brief
                                                                              *   Called each time the trackball interrupt IN endpoint should be checked.
                                                                              *****************************************************************************/
static void PollTimeout(void)
{
    pollTimerDone = true;
}

static void SuspendTimeout(void)
{
    suspendTimerDone = true;
}

/**************************************************************************/ /**
                                                                              * @brief
                                                                              *   Check if the trackball has been moved. And print the coords to the lcd.
                                                                              *****************************************************************************/
static void CheckPadSwipe(vec2int *coords)
{
    int8_t x = 0;
    int8_t y = 0;
    char padDir[3];

    int readBytes = USBH_ReadB(&ep, usbData, ep.epDesc.wMaxPacketSize, PAD_IN_REPORT_LEN); /* Read data from USB */

    if (readBytes != PAD_IN_REPORT_LEN)
    {
        // Handle error or log error_msg if needed
        return;
    }

    // Extract x and y values
    x = usbData[1];
    y = usbData[2];

    coords->x = usbData[1];
    coords->y = usbData[2];

    // Determine pad direction
    if (x == 0 && y == 0)
    {
        // No movement detected
        return;
    }

    // Determine direction based on signs of x and y
    if (y < 0)
    {
        if (x < 0)
        {
            sprintf(padDir, "LU"); // Left-Up
        }
        else if (x > 0)
        {
            sprintf(padDir, "RU"); // Right-Up
        }
        else
        {
            sprintf(padDir, "U"); // Up
        }
    }
    else if (y > 0)
    {
        if (x < 0)
        {
            sprintf(padDir, "LD"); // Left-Down
        }
        else if (x > 0)
        {
            sprintf(padDir, "RD"); // Right-Down
        }
        else
        {
            sprintf(padDir, "D"); // Down
        }
    }
    else
    {
        if (x < 0)
        {
            sprintf(padDir, "L"); // Left
        }
        else if (x > 0)
        {
            sprintf(padDir, "R"); // Right
        }
    }

    /*
    // Display direction and values
    SegmentLCD_Write(padDir);
    SegmentLCD_Number(x);
    SegmentLCD_LowerNumber(y);
    */
}
