#pragma once

#include <acpi/acpi_tables.h>
#include <string.h>

extern rsdp2_t* rsdp;
extern xsdt_t* xsdt;
extern rsdt_t* rsdt;
uint8_t acpi_validate_rsdp_checksum(void);
uint8_t acpi_validate_sdt_checksum(sdt_header_t* sdt);
sdt_header_t* acpi_find_table(const char sig[4], const uint64_t hhdm);
void acpi_check_table_signature(void);
