// tty.h
#pragma once

#include <limine.h>
#include <drivers/tty/tty.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define PSF_MAGIC_NUM 0x0436
#define FONT_SCALE 2

// requires the unscii font in ./other/unscii.psf
// to be linked to the kernel binary
extern uint8_t _binary___other_unscii_psf_start[];
extern uint8_t _binary___other_unscii_psf_end[];
extern volatile struct limine_framebuffer_request framebuffer_request;
extern struct limine_framebuffer* fb;

typedef struct {
	uint16_t magic;
	uint8_t mode;
	uint8_t charsize;
} __attribute__((packed)) psf1_font_header_t;

typedef struct {
	psf1_font_header_t* header;
	uint8_t* glyphs;
	uint16_t num_glyphs;
	uint8_t width, height;
}  psf1_font_t;

void font_init(void);
void limine_putblock(uint32_t pos_x, uint32_t pos_y, uint32_t scale, uint32_t color);
void limine_putpixel(uint32_t pos_x, uint32_t pos_y, uint32_t color);
void tty_init(void);
void tty_setcolor(const uint32_t color);
void tty_putcharat(psf1_font_t* font,
				 uint32_t c,
				 uint32_t pos_x,
				 uint32_t pos_y,
				 uint32_t fg,
				 uint32_t bg);
void tty_scroll(unsigned char c, size_t pos_x, size_t pos_y);
void tty_putchar(char c);
void kwrite(const char* data);
