// tty.c
#include <drivers/tty/tty.h>
#include <limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

extern volatile struct limine_framebuffer_request framebuffer_request;
extern struct limine_framebuffer* fb;

static uint64_t fb_pitch_pixels;
static psf1_font_t font_inst;
static psf1_font_t* font = &font_inst;

static size_t tty_x;
static size_t tty_y;
static uint32_t tty_fg_color;
static uint32_t tty_bg_color;

void font_init(void)
{
    if (!fb || !fb->address) return;
    fb_pitch_pixels = fb->pitch / 4;
    font->header = (psf1_font_header_t*)_binary___other_unscii_psf_start;
    if (font->header->magic != PSF_MAGIC_NUM) return;
    font->width = 8;
    font->height = font->header->charsize;
    font->num_glyphs = (font->header->mode & 1) ? 512 : 256;
    font->glyphs = _binary___other_unscii_psf_start + 4;
}
// add boundary checking
void limine_putpixel(uint32_t pos_x, uint32_t pos_y, uint32_t color)
{
    if ((pos_x < fb->width) && (pos_y < fb->height)) {
        uint32_t* fb_addr = (uint32_t*)fb->address;
        fb_addr[pos_y * fb_pitch_pixels + pos_x] = color;
    }
}

void limine_putblock(uint32_t pos_x,
                     uint32_t pos_y,
                     uint32_t scale,
                     uint32_t color)
{
    for (uint32_t i = 0; i < scale; i++) {
        for (uint32_t j = 0; j < scale; j++) {
            // Draws to the linear framebuffer
            limine_putpixel(pos_x + i, pos_y + j, color);
        }
    }
}

void tty_init(void)
{
    tty_x = 0;
    tty_y = 0;
    tty_fg_color = 0xffffff;
    tty_bg_color = 0x000000;
}

void tty_putcharat(psf1_font_t* font,
                   uint32_t c,
                   uint32_t pos_x,
                   uint32_t pos_y,
                   uint32_t fg,
                   uint32_t bg)
{
    //	if(!font || !font->glyphs) return;
    if (c >= font->num_glyphs) c = 0;

    uint8_t* glyph = font->glyphs + (c * font->height);
    for (uint32_t row = 0; row < font->header->charsize; row++) {
        uint8_t bits = glyph[row];
        for (uint32_t col = 0; col < 8; col++) {
            limine_putblock(pos_x + col * FONT_SCALE,
                            pos_y + row * FONT_SCALE,
                            FONT_SCALE,
                            (bits & (0x80 >> col)) ? fg : bg);
        }
    }
}

void tty_setcolor(const uint32_t color)
{
    tty_fg_color = color;
}

void kwrite(const char* data)
{
    tty_fg_color = 0xffffff;
    for (uint32_t i = 0; i < strlen(data); i++) {
        if (data[i] == '\n') {
            tty_x = 0;
            tty_y += font->height * FONT_SCALE;
        } else {
            tty_putcharat(
              font, data[i], tty_x, tty_y, tty_fg_color, tty_bg_color);
            tty_x += font->width * FONT_SCALE;
        }
    }
}
