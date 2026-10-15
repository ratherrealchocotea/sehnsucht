#include <acpi/acpi_tables.h>
#include <acpi/acpi.h>
#include <acpi/madt.h>
#include <helpers.h>
#include <drivers/tty/tty.h>
#include <stdint.h>

madt_t* madt;
uint32_t lapic_addr;
void acpi_parse_madt(void)
{
    if (acpi_validate_sdt_checksum(&madt->sdt) != 0) {
        kwrite("The MADT is invalid\n");
        halt_catch_fire();
    }
    lapic_addr = madt->apic_addr;
}
