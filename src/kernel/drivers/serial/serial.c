// serial.c
#include <drivers/serial/serial.h>
#include <stdint.h>

#define COM1 0x3f8

uint32_t serial_init()
{
    _outb(COM1 + 1, 0x00); // Disable interrupts
    _outb(COM1 + 3, 0x80); // Set baud rate divisor
    _outb(COM1 + 0, 0x03); // Set divisor to 3 (38400 baud) (lo byte)
    _outb(COM1 + 1, 0x00); // (hi byte)
    _outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    _outb(COM1 + 2, 0xc7); // Enable FIFO, clear them, with 14-byte threshold
    _outb(COM1 + 4, 0x0b); // IRQs enabled, RTS/DSR set
    _outb(COM1 + 4, 0x1e); // Set in loopback mode, test the serial chop
    _outb(COM1 + 0,
          0xae); // Send byte 0xae and check if serial returns the same byte

    if (_inb(COM1 + 0) != 0xae)
        return 1; // If this doesn't work, the serial is faulty

    // Set to normal operating mode, if not faulty
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits
    // enabled)
    _outb(COM1 + 4, 0x0f);
    return 0;
}

char serial_readc()
{
    while ((_inb(COM1 + 5) & 1) == 0);
    return _inb(COM1);
}

void serial_writec(const char data)
{
    while ((_inb(COM1 + 5) & 0x20) == 0);
    _outb(COM1, data);
}

void serial_write(const char* data)
{
    while (*data) {
        if (*data == '\n') serial_writec('\r');
        serial_writec(*data++);
    }
}
