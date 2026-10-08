#pragma once
#include <stdint.h>

static inline uint64_t addr_convert_p2v(const uint64_t addr, const size_t hddm)
{
	return addr + hddm;
}

static inline uint64_t addr_convert_v2p(const uint64_t addr, const size_t hddm)
{
	return addr - hddm;
}
