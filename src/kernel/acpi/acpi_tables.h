#pragma once

#include <limine.h>

struct limine_rsdp_request {
  uint64_t id[4];
  uint64_t revision;
  struct limine_rsdp_response* response;
};

struct limine_rsdp_response {
  uint64_t revision;
  uint64_t address; // physical address of the RSDP
};
