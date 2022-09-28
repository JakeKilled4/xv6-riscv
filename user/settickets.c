#include "kernel/types.h"
#include "user/user.h"
#include "kernel/pstat.h"

int
main(int argc, char *argv[])
{ 
    
  struct pstat p;
    
  getpinfo(&p);
  for(int i = 0;i< sizeof(p.pid)/sizeof(int);i++){
    if(p.inuse[i] == 1){
      fprintf(2,"%d %s %d %d\n", p.pid[i], p.name[i], p.tickets[i],p.time[i]);
    }
  }
  fprintf(2,"\n");
	fprintf(2,"%d \n",uptime());
 
  exit(0);
}
