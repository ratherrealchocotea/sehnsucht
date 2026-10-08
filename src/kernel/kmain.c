// kmain.c
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

#include <string.h>
#include <drivers/tty/tty.h>
#include <drivers/serial/serial.h>
#include <interrupts/int.h>

#include <helpers.h>

extern void _disable_int(void);
extern void _init_pic(void);

//static uint64_t hhdm_offset;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

inline static void halt_catch_fire(void)
{
	for(;;) asm volatile("pause");
}

struct limine_framebuffer* fb;

void kmain(void)
{
	// Ensure the bootloader understands our base revision
	if(LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
		// Add errors here
		halt_catch_fire();
	}
	if(framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
		halt_catch_fire();
	}
	fb = framebuffer_request.response->framebuffers[0];
   
	if(rsdp_request.response == NULL) {
		kwrite("No RSDP provided by Limine");
		halt_catch_fire();
	}

	_disable_int();
	_init_pic();	
	idt_init(); // also enables interrupts
	font_init();
	tty_init();
	
	asm volatile (
		"movl 7, %edx\n"
		"movl 0, %ecx\n"
		"divl %ecx\n"
	);

	kwrite("Booted okay so far\n");
    halt_catch_fire();
}
