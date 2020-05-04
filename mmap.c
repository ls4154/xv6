#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "mman.h"
#include "x86.h"

int
do_mmap(void *addr, int length, int prot, int flags, int fd, int offset)
{
  int i;
  int npages;
  struct file *f;
  struct inode *ip = 0;
  struct mmap_info *mmi;
  struct proc *curproc = myproc();

  // non page-aligned value not supported
  if((int)addr % PGSIZE || (int)length % PGSIZE)
    return -1;

  // PROT_READ required
  if(!(prot & PROT_READ))
    return -1;

  // shared map not supported
  if(flags & MAP_SHARED)
    return -1;

  if((flags & MAP_ANONYMOUS) && fd != -1)
    return -1;

  // get free mmap_info struct
  for(i = 0; i < NMMAP; i++){
    if(curproc->mmap[i].addr == 0){
      mmi = &curproc->mmap[i];
      goto found;
    }
  }
  return -1;

found:
  if(addr == 0)
    addr = (void*)(MMAPBASE + curproc->mmap_pgcnt * PGSIZE);

  npages = PGROUNDUP(length) / PGSIZE;

  mmi->addr = addr;
  mmi->sz = length;
  mmi->ip = 0;
  mmi->off = offset;
  mmi->prot = prot;

  if(!(flags & MAP_ANONYMOUS)){
    if(fd < 0 || fd >= NOFILE ||(f=myproc()->ofile[fd]) == 0)
      goto fail;
    ip = f->ip;
    mmi->ip = ip;
    if((prot & PROT_READ) && !(f->readable))
      goto fail;
    if((prot & PROT_WRITE) && !(f->writable))
      goto fail;
  }

  if(flags & MAP_POPULATE){
    char *mem;
    int perm = PTE_U;
    if(prot & PROT_WRITE)
      perm |= PTE_W;

    for(i = 0; i < npages; i++){
      // allocate and map single page
      mem = kalloc();
      if(mem == 0){
        cprintf("mmap out of memory\n");
        // TODO: free and unmap
        goto fail;
      }
      memset(mem, 0, PGSIZE);
      mappages(curproc->pgdir, (char*)addr + i * PGSIZE, PGSIZE, V2P(mem), perm);
      // load file data to memory
      if(!(flags & MAP_ANONYMOUS)){
        begin_op();
        ilock(ip);
        // readi handle out of file size so no check here
        readi(ip, mem, offset + i * PGSIZE, PGSIZE);
        iunlock(ip);
        end_op();
      }
    }
  }

  curproc->mmap_pgcnt += npages;
  return (int)addr;
fail:
  mmi->addr = 0;
  mmi->sz = 0;
  mmi->ip = 0;
  mmi->off = 0;
  mmi->prot = 0;
  return -1;
}

int
do_munmap(void *addr)
{
  int i;
  struct inode *ip;
  struct mmap_info *mmi;
  struct proc *curproc = myproc();
  int npages;

  for(i = 0; i < NMMAP; i++){
    if(curproc->mmap[i].addr == addr){
      mmi = &curproc->mmap[i];
      goto found;
    }
  }
  return -1;
found:
  ip = mmi->ip;
  npages = PGROUNDUP(mmi->sz) / PGSIZE;

  for(i = 0; i < npages; i++){
    char *mem = mmi->addr + i * PGSIZE;
    char *kmem;
    // check page allocated
    if(walkpgdir(curproc->pgdir, mem, 0) == 0)
      continue;
    kmem = uva2ka(curproc->pgdir, mem);
    // file writeback
    if(ip){
      begin_op();
      ilock(ip);
      if (i == npages - 1)
        writei(ip, mem, mmi->off + i * PGSIZE, mmi->sz % PGSIZE);
      else
        writei(ip, mem, mmi->off + i * PGSIZE, PGSIZE);
      iunlock(ip);
      end_op();
    }
    kfree(kmem);
    unmappage(curproc->pgdir, mem);
  }

  mmi->addr = 0;
  mmi->sz = 0;
  mmi->ip = 0;
  mmi->off = 0;
  mmi->prot = 0;
  return 0;
}

int
check_pgflt_mmap(void *addr)
{
  int i;
  struct mmap_info *mmi;
  struct proc *curproc = myproc();
  char *mem;
  int perm = PTE_U;
  for(i = 0; i < NMMAP; i++){
    mmi = &curproc->mmap[i];
    if(mmi->addr <= addr && mmi->addr + mmi->sz > addr)
      goto found;
  }
  return -1;
found:
  if(curproc->tf->err & 2){
    if(!(mmi->prot & PROT_WRITE))
      return -1;
    perm |= PTE_W;
  }

  mem = kalloc();
  if(mem == 0){
    cprintf("mmap out of memory\n");
    return -1;
  }
  memset(mem, 0, PGSIZE);
  mappages(curproc->pgdir, (char*)PGROUNDDOWN((uint)addr), PGSIZE, V2P(mem), perm);
  if(mmi->ip){
    begin_op();
    ilock(mmi->ip);
    // readi handle out of file size so no check here
    readi(mmi->ip, mem, mmi->off + PGROUNDDOWN((uint)addr) - (uint)mmi->addr, PGSIZE);
    iunlock(mmi->ip);
    end_op();
  }

  return 0;
}
