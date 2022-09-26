#include "kernel/types.h"
#include "user/user.h"
#include "kernel/pstat.h"

int
main(int argc, char *argv[])
{ 
    
  struct pstat p;
    
  getpinfo(&p);
  for(int i = 0;i< sizeof(p.pid)/sizeof(int);i++)
    fprintf(2,"%d ",p.pid[i]);
  fprintf(2,"\n");
  /*  
 
  int r = settickets(10);
  for(int i = 0;i<1000000000;i++){
  }
  fprintf(2, "Funciona, recibido: %d\n",r);
  */
  
  exit(0);
}
