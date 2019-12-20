#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
  int pid;
  int value;

  if(argc != 3){
    printf(2, "usage: setnice pid value...\n");
    exit();
  }

  pid = atoi(argv[1]);
  value = atoi(argv[2]);
  setnice(pid, value);

  exit();
}
