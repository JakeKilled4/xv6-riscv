#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{ 
  int r = settickets(10);
  fprintf(2, "Funciona, recibido: %d",r);
  exit(0);
}
