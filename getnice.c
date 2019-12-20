#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
  int pid;

  if(argc != 2){
    printf(2, "usage: getnice pid...\n");
    exit();
  }

  pid = atoi(argv[1]);
  printf(1, "%d\n", getnice(pid));

  exit();
}
