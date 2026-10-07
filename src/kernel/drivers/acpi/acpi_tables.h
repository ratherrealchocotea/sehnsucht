#pragma once
// https://github.com/TheUltimateFoxOS/horizon/blob/main/core/include/acpi/acpi_tables.h
typedef struct {
	uint8_t signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t oem_table_id;
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} sdt_header __attribute__((packed));

typedef struct {
	uint8_t entry_type;
	uint8_t record_length;
} madt_record_t __attribute__((packed));

typedef struct {
	sdt_header sdt;
	uint32_t apic_addr;
	uint32_t flags;
} madt_t __attribute__((packed));