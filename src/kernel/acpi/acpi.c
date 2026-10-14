#include <acpi/acpi.h>
#include <acpi/acpi_tables.h>
#include <drivers/tty/tty.h>
#include <helpers.h>
#include <string.h>

rsdp2_t* rsdp;
xsdt_t* xsdt;
rsdt_t* rsdt;

uint8_t acpi_validate_rsdp_checksum(void)
{
    uint8_t* p = (uint8_t*)rsdp;
    uint32_t len = rsdp->rsdp.revision >= 2 ? 36 : 20;
    uint8_t sum = 0;
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
    if (xsdt) {
        uint32_t n = (xsdt->sdt.len - sizeof(sdt_header_t)) / 8;
        for (uint32_t i = 0; i < n; i++) {
            sdt_header_t* h = (sdt_header_t*)(uintptr_t)addr_convert_p2v(
              xsdt->entries[i], hhdm);
            if (*(uint32_t*)h->signature == want) return h;
        }
    } else if (rsdt) {
        uint32_t n = (rsdt->sdt.len - sizeof(sdt_header_t)) / 4;
        for (uint32_t i = 0; i < n; i++) {
            // check if the physical to virtual conversion
            // is really necessary
            sdt_header_t* h = (sdt_header_t*)(uintptr_t)addr_convert_p2v(
              rsdt->entries[i], hhdm);
            if (*(uint32_t*)h->signature == want) return h;
        }
    }
    return 0;
}

void acpi_check_table_signature(void)
{
    if (xsdt) {
        if (memcmp(xsdt->sdt.signature, "XSDT", 4)) {
            kwrite("The XSDT signature is fuaaarked\n");
            halt_catch_fire();
        }
        if (xsdt->sdt.len < sizeof(sdt_header_t)) {
            kwrite("XSDT length is too small\n");
        }
    } else if (rsdt) {
        if (memcmp(rsdt->sdt.signature, "RSDT", 4)) {
            kwrite("The RSDT signature is fuaaarked\n");
            halt_catch_fire();
        }
        if (rsdt->sdt.len < sizeof(sdt_header_t)) {
            kwrite("RSDT length is too small\n");
            halt_catch_fire();
        }
    } else {
        kwrite("NO XSDT or RSDT present\n");
        halt_catch_fire();
    }
}
