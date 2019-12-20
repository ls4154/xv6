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

int
do_mmap(void *addr, int length, int prot, int flags, int fd, int offset)
{
  return 0;
}

int
do_munmap(void *addr)
{
  return 0;
}

int
check_pgflt_mmap(void *addr)
{
  return 0;
}
