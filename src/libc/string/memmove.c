// memmove.c
#include <string.h>

void*
memmove(void* dst_ptr, const void* src_ptr, size_t size)
{
  unsigned char* dst = (unsigned char*)dst_ptr;
  const unsigned char* src = (const unsigned char*)src_ptr;

  if (dst < src) {
    for (size_t i = 0; i < size; i++)
      dst[i] = src[i];
  } else {
    for (size_t i = size; i != 0; i--)
      dst[i - 1] = src[i - 1];
  }

  return dst_ptr;
}
