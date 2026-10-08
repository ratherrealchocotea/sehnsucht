#pragma once

#include <stdint.h>

typedef struct {
	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_addr;
} __attribute__((packed)) rsdp_t;

typedef struct {
	char signature[4];
	uint32_t len;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[6];
	char oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} __attribute__((packed)) std_header_t;

typedef struct {
	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t len;
	uint64_t xsdt_addr;
	uint8_t extended_checksum;
	uint8_t reserved[3]
} __attribute__((packed)) xsdp_t;

typedef struct {
	std_header_t sdt;
	uint32_t apic_addr;
	uint32_t flags;
} __attribute__((packed)) madt_t;
