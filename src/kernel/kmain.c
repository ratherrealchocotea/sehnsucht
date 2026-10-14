// kmain.c
#include <limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/serial/serial.h>
#include <drivers/tty/tty.h>
#include <interrupts/int.h>
#include <string.h>

#include <acpi/acpi.h>

#include <helpers.h>

extern void _disable_int(void);
extern void _init_pic(void);
extern void _disable_pic(void);
extern rsdp2_t* rsdp;
extern xsdt_t* xsdt;
static uint64_t hhdm_offset;

struct limine_framebuffer* fb;

__attribute__((used, section(".limine_requests"))) static volatile uint64_t
  limine_base_revision[] = LIMINE_BASE_REVISION(6);
/* __attribute__((used, section(".limine_requests"))) */
/* static volatile struct limine_framebuffer_request framebuffer_request = { */
/*     .id = LIMINE_FRAMEBUFFER_REQUEST_ID, */
/*     .revision = 0 */
/* }; */

__attribute__((
  used,
  section(".limine_requests"))) volatile struct limine_framebuffer_request
  framebuffer_request = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 0 };
__attribute__((used,
               section(".limine_requests"))) volatile struct limine_hhdm_request
  hhdm_request = { .id = LIMINE_HHDM_REQUEST_ID, .revision = 0 };

__attribute__((used,
               section(".limine_requests"))) volatile struct limine_rsdp_request
  rsdp_request = { .id = LIMINE_RSDP_REQUEST_ID, .revision = 0 };

__attribute__((used,
               section(".limine_requests_start"))) static volatile uint64_t
  limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end"))) static volatile uint64_t
  limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

void kmain(void)
{

    // Ensure the bootloader understands our base revision
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        // Add errors here
        halt_catch_fire();
    }
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        halt_catch_fire();
    }

    fb = framebuffer_request.response->framebuffers[0];

    hhdm_offset = hhdm_request.response->offset;
    _disable_int();
    _init_pic();
    _disable_pic();
    idt_init(); // also enables interrupts
    font_init();
    tty_init();
    if (rsdp_request.response == NULL) {
        kwrite("No RSDP provided by Limine");
        halt_catch_fire();
    }

    rsdp = (rsdp2_t*)rsdp_request.response->address;

    if (rsdp->rsdp.revision >= 2 && rsdp->xsdt_addr) {
        xsdt =
          (xsdt_t*)(uintptr_t)addr_convert_p2v(rsdp->xsdt_addr, hhdm_offset);
    } else {
        rsdt = (rsdt_t*)(uintptr_t)addr_convert_p2v(rsdp->rsdp.rsdt_addr,
                                                    hhdm_offset);
    }

    if ((acpi_validate_rsdp_checksum() & 0xff) != 0) {
        kwrite("ACPI initialization failed");
        halt_catch_fire();
    }

    if (memcmp(rsdp->rsdp.signature, "RSD PTR ", 8)) {
        kwrite("Failed to verify RSDP signature");
        halt_catch_fire();
    }

    if (rsdp->rsdp.revision >= 2 && rsdp->xsdt_addr) {
        xsdt = (xsdt_t*)rsdp->xsdt_addr;
    }

    acpi_check_table_signature();

    kwrite("Booted okay so far\n");
    halt_catch_fire();
}
