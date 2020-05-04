#include <stdint.h>
#define __IO volatile
#define __IM volatile
#define __OM volatile
#define __IOM volatile
#define __I volatile
// Macros to enable/disable global interrupts
#define enable_interrupts() asm(" cpsie i ")
#define disable_interrupts() asm(" cpsid i ")
// macro for putting the CPU in to sleep mode
#define cpu_sleep() asm(" wfi ")
// Some useful bitmasks
