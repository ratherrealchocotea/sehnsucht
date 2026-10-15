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
#include <acpi/madt.h>
#include <helpers.h>

extern void _disable_int(void);
extern void _init_pic(void);
extern void _disable_pic(void);

extern void _enable_apic(void);
extern uint64_t _get_apic_mmio_base(void);
extern void _lapic_enable(volatile uint32_t* lapic);

extern rsdp2_t* rsdp;
extern xsdt_t* xsdt;
extern madt_t* madt;
extern uint32_t lapic_addr;

uint64_t hhdm_offset;

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

    // Code related to the ACPI

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

    // code related to the APIC
    acpi_parse_madt();

    volatile uint32_t* lapic =
      (volatile uint32_t*)(uintptr_t)(lapic_addr + hhdm_offset);
    _enable_apic();
    _lapic_enable(lapic);

    // Verify the APIC is really how it should be
    uint32_t svr = lapic[0xf0 / 4];
    if (!(svr & (1 << 8))) {
        kwrite("LAPIC SVR enable failed");
        halt_catch_fire();
    }

    uint32_t ver = lapic[0x30 / 4];
    if (ver == 0 || ver == 0xffffffff) {
        kwrite("The LAPIC is not responding!!");
        halt_catch_fire();
    }

    kwrite("Booted okay so far\n");
    halt_catch_fire();
}
