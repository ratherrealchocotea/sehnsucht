#include <acpi/acpi.h>
#include <acpi/acpi_tables.h>
#include <helpers.h>
#include <string.h>
#include <drivers/tty/tty.h>

rsdp2_t* rsdp;
xsdt_t* xsdt;

uint8_t acpi_validate_rsdp_checksum(void)
{
    uint8_t *p = (uint8_t *)rsdp;
    uint32_t len = rsdp->rsdp.revision >= 2 ? 36 : 20;
    uint8_t  sum = 0;
    for (uint32_t i = 0; i < len; i++) sum += p[i];

    return sum;
}

uint8_t acpi_validate_sdt_checksum(sdt_header_t* sdt)
{
    uint8_t sum = 0;
    for (uint32_t i = 0; i < sdt->len; i++) sum += ((uint8_t*)sdt)[i];

    return sum == 0;
}

sdt_header_t* acpi_find_table(const char sig[4], const uint64_t hhdm)
{
    uint32_t want = *(const uint32_t*)sig;
    uint32_t n = (xsdt->sdt.len - sizeof(sdt_header_t)) / 8;
    for (uint32_t i = 0; i < n; i++) {
		sdt_header_t *h = (sdt_header_t *)(uintptr_t)addr_convert_p2v(xsdt->entries[i], hhdm);
        if (*(uint32_t*)h->signature == want) return h;
    }
    return 0;
}

void acpi_check_xsdt_signature(void)
{
    if (!xsdt) {
        kwrite("No XSDT present");
        halt_catch_fire();
    }
    if (memcmp(xsdt->sdt.signature, "XSDT", 4)) {
        kwrite("XSDT signature fuaaarked");
        halt_catch_fire();
    }
    if (xsdt->sdt.len < sizeof(sdt_header_t)) {
        kwrite("XSDT length is too small");
    }
}
