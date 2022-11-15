#ifndef __VMA_H__
#define __VMA_H__

#include "types.h"
#include "file.h"

struct vma{
  uint8 in_use;           // 0 if not used 1 if is in use
  uint64 start_addr;      // Start of memory map (page aligned)
  uint64 end_addr;        // End of memory map (page aligned)
  uint8 prot;             // Flags for read and write
  uint8 flags;            // Flags for private and share 
  int offset;             // Offset inside the file
  struct file* f;         // File to map this memory
  uint64 size;            // Size of the file when map
};

#endif

