#pragma once
#include <types.h>

/* Basic I/O Functions. */

/* Get input byte from port. */
extern uint8_t inb(uint16_t port);

/* Get input word from port. */
extern uint16_t inw(uint16_t port);

/* Output byte to port. */
extern void outb(uint16_t port, uint8_t val);

/* Output word to port. */
extern void outw(uint16_t port, uint16_t val);