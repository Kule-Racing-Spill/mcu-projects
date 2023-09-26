#define USB_DEVICE       // Compile the stack for device mode.
#define NUM_EP_USED 1    // Your application use 'n' endpoints in
                         // addition to endpoint 0.
#define NUM_APP_TIMERS 1 // Your application needs 'n' timers

#define DEBUG_USB_API    // Turn on API debug diagnostics.

// Some utility functions in the API needs printf. These
// functions have "print" in their name. This macro enables
// these functions.
#define USB_USE_PRINTF   // Enable utility print functions.

// Define a function for transmitting a single char on the serial port.
extern int RETARGET_WriteChar(char c);
#define USER_PUTCHAR  RETARGET_WriteChar

#define USB_TIMER USB_TIMERn  // Select which hardware timer the USB stack
                              // is allowed to use. Valid values are n=0,1,2...
                              // corresponding to TIMER0, TIMER1, ...
                              // If not specified, TIMER0 is used

#define USB_VBUS_SWITCH_NOT_PRESENT  // Hardware does not have a VBUS switch

#define USB_CORECLK_HFRCO   // Devices supporting crystal-less USB can use
                            // HFRCO as core clock, default is HFXO
