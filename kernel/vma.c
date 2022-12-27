#include "vma.h"
#include "file.h"
#include "mcntl.h"
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"


// VMA is sorted by end_addr from bigger to lower value
struct vma* mmap(struct vma* vma, struct file *f, uint len, int prot, int flags, int offset){

  struct vma * v;
  struct vma * temp_v = 0;
  uint64 temp_start_addr = 0;
  uint64 last_start_addr = TRAPFRAME;

  // Specified in mmap documentation
  if(offset % PGSIZE != 0) 
    return 0;

  // offset can't be greater than f size and f must be inode
  if(offset >= f->ip->size || f->type != FD_INODE || len == 0)
    return 0;

  // Map completes pages
  len = PGROUNDUP(len);  

  // Map fail if file is not readable and map is to read and if the file 
  // is not writable and the map is shared with write permissions
  if((!f->readable && (prot & PROT_READ)) || 
    (!f->writable && (prot & PROT_WRITE) && (flags & MAP_SHARED))) 
    return 0;

  // Iterate over all the maps in use
  for(v = vma; v < &vma[MAXMAPS] && v->in_use; v++){

    if(temp_v == 0 && last_start_addr - v->end_addr >= len){ 
      temp_v = v;
      temp_start_addr = v->end_addr; 
    }
    last_start_addr = v->start_addr;
  }
  
  // No more maps
  if(v == &vma[MAXMAPS]) 
    return 0;

  // If we found a space (v now point to the first space not in use)
  // Else add at the end (v now point to the first space not in use)
  if(temp_v != 0){
    while(v > temp_v){
      *v = *(v-1);
      v--;
    }
  }
  else{ 
    temp_start_addr = last_start_addr - len;
  }

  v->in_use = 1;
  v->start_addr = temp_start_addr;
  v->end_addr = v->start_addr + len;
  v->f = f;  
  v->flags = flags;
  v->offset = offset;
  v->size = f->ip->size;

  // In riscv if you have write access you nedd
  // read access
  if(prot & PROT_WRITE) prot |= PROT_READ;
  v->prot = prot;

  // Increment references to the file
  filedup(f);

  return v;
}


  // Check if there is a map for that direcction
struct vma * find_map(struct vma* vma, uint64 addr){
  struct vma * v;

  for(v = vma; v < &vma[MAXMAPS] && v->in_use; v++){

    // mmap documentation say that when the address 
    // in a page but it is out of the file return zeros
    if(addr >= v->start_addr && addr < v->end_addr)
      return v; 
  }
  return 0;
}

int munmap(struct vma* vma, pagetable_t pagetable, uint64 start_addr, uint len){
  
  uint64 end_addr;
  struct vma* v;

  // Must be page aligned
  if(start_addr % PGSIZE != 0) return -1; 
  
  // Specified in munmap documentation 
  if(len == 0) return -1;

  // Unmap complete pages
  len = PGROUNDUP(len);
  end_addr = start_addr + len;

  for(v = vma; v < &vma[MAXMAPS] && v->in_use;v++){

    // If we find a map that start at the same page
    if(v->start_addr == start_addr){

      // Out of the map
      if(end_addr > v->end_addr)
        return -1;

      // Free the pages and if flags == MAP_SHARED save the file      
      free_pages(pagetable, start_addr, len, (v->flags & MAP_SHARED) ? v->f : 0, v->offset, v->size);

      // Free the vma or just update the range
      if(end_addr == v->end_addr){ 
        fileclose(v->f);

        // Delete the space (move all one position left, now v point to the last 
        // valid map, that now should be 0 because we have moved one position left)
        while((v+1) < &vma[MAXMAPS] && (v+1)->in_use != 0){
          *v = *(v+1);
          v++;
        }

        v->in_use = 0;
        v->start_addr = 0;
        v->end_addr = 0;
        v->f = 0;
        v->flags = 0;
        v->prot = 0;
        v->offset = 0;
        v->size = 0;

      }
      else{ 
        v->offset += (end_addr - v->start_addr);
        v->start_addr = end_addr;
      }  
      return 0;
    }

    // If we find a map that end at the same page
    // The case that the complete map have to be free
    // is in the last if
    if(end_addr == v->end_addr){ 

      // Out of the map
      if(start_addr < v->start_addr)
        return -1;

      // Free the pages and if flags == MAP_SHARED save the file      
      free_pages(pagetable, start_addr, len, (v->flags & MAP_SHARED) ? v->f : 0, v->offset + start_addr - v->start_addr, v->size);
 
      v->end_addr = start_addr;
      return 0;
    }
  } 
  // Map not found or the address for munmap is in the middle of the map
  return -1;
}




