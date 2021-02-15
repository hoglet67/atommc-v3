#ifndef _IO

// Mask applied to register address bits.
#define ADDRESS_MASK	0x07

#define LEDPINSOUT() TRISCbits.TRISC0 = 0; TRISCbits.TRISC1 = 0;

extern void pico_led(int state);
#define REDLEDON()
#define REDLEDOFF()
#define GREENLEDON() pico_led(1);
#define GREENLEDOFF() pico_led(0);

#define  V4HARDWARE

#ifdef V4HARDWARE
// V4 hardware has IRQ on different pins, freeing up the serial port
#define ASSERTIRQ()  PORTAbits.RA4 = 0; TRISAbits.TRISA4 = 0;
#define RELEASEIRQ() TRISAbits.TRISA4 = 1;
#else
#define ASSERTIRQ()  PORTCbits.RC6 = 0; TRISCbits.TRISC6 = 0;
#define RELEASEIRQ() TRISCbits.TRISC6 = 1;
#endif

#define ACTIVITYSTROBE(x)

extern unsigned int was_write();
#define WASWRITE was_write()

extern unsigned int write_address();
#define LatchAddressIn() { LatchedAddressLast=write_address(); }
extern void ReadDataPort();
extern void WriteDataPort(int value);

extern void redSignal(char);

#define _IO
#endif
