#ifndef __VMA_H__
#define __VMA_H__

#include "types.h"
#include "file.h"

struct vma{
  uint8 used;             // 0 if not used 1 if is in use
  uint64 start_addr;      // Start of memory map
  uint len;                // Length to map
  uint8 prot;             // Flags for read and write
  uint8 flags;            // Flags for private and share 
  int offset;             // Offset inside the file
  struct file* f;         // File to map this memory
};

#endif

