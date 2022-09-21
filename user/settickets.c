#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{ 
  int r = settickets(10);
  for(int i = 0;i<1000000000;i++){
  }
  fprintf(2, "Funciona, recibido: %d\n",r);
  
  exit(0);
}
