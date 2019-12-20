#include "types.h"
#include "defs.h"
#include "param.h"

int
sys_mmap(void)
{
  uint addr;
  int len;
  int prot;
  int flags;
  int fd;
  int off;

  if(argint(0, (int*)&addr) < 0)
    return -1;

  if(argint(1, &len) < 0)
    return -1;

  if(argint(2, &prot) < 0)
    return -1;

  if(argint(3, &flags) < 0)
    return -1;

  if(argint(4, &fd) < 0)
    return -1;

  if(argint(5, &off) < 0)
    return -1;

  return do_mmap((void*)addr, len, prot, flags, fd, off);
}

int
sys_munmap(void)
{
  uint addr;

  if(argint(0, (int*)&addr) < 0)
    return -1;

  return do_munmap((void*)addr);
}
