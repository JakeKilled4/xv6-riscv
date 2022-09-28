#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/pstat.h"

void spin() {
  volatile uint x = 0;
  volatile uint y = 0;
  
  while (x < 1000000) {
    y = 0;
    while (y < (10000)) {
      y++;
    }
    x++;
  }
}

// Print the ticks of each process with this pid's 
void printticks(int pid1, int pid2, int pid3){
  struct pstat pi = {0};
  getpinfo(&pi);
  int i;
  int time1=0, time2=0, time3=0;
  for (i = 0; i < NPROC; i++) {
    if(pi.pid[i] == pid1)      time1 = pi.time[i];
    else if(pi.pid[i] == pid2) time2 = pi.time[i]; 
    else if(pi.pid[i] == pid3) time3 = pi.time[i]; 
  }
  fprintf(2,"%d \t \t %d \t\t %d\n",time1,time2,time3);
}


void printpinfo(int pid)
{
  struct pstat pi = {0};
  getpinfo(&pi);
  int i;
  for (i = 0; i < NPROC; i++) {
    if(pi.pid[i] == pid) {
      fprintf(2, "Number of tickets that PID %d has: %d\n", pid, pi.tickets[i]);
      fprintf(2, "Number of ticks that PID %d has: %d\n", pid, pi.time[i]);
      fprintf(2, "Is the process with PID %d in use? (0 or 1): %d\n", pid, pi.inuse[i]);
    }
  }
}

int
main(int argc, char *argv[])
{

  settickets(40);
  int pid1, pid2, pid3;
  if ((pid1 = fork()) == 0) {
    int pp1 = getpid();
    fprintf(2, "Process started with PID %d\n\n", pp1);
    settickets(10);
    spin();
    printpinfo(pp1);
    fprintf(2, "Process with PID %d finished!\n\n", pp1);
    exit(0); 
  }
  if ((pid2 = fork()) == 0) {
    int pp2 = getpid();
    fprintf(2, "Process started with PID %d\n\n", pp2);
    settickets(20);
    spin();
    printpinfo(pp2);
    fprintf(2, "Process with PID %d finished!\n\n", pp2);
    exit(0);
  }
  if ((pid3 = fork()) == 0) {
    int pp3 = getpid();
    fprintf(2, "Process started with PID %d\n\n", pp3);
    settickets(30);
    spin();
    printpinfo(pp3);
    fprintf(2, "Process with PID %d finished!\n\n", pp3);
    exit(0);
  }

  volatile uint64 count = 0;
  // Little delay to start all childs
  while(count<100000000) count++;
  fprintf(2,"Ticks of each process with [pid, ticks]:\n");
  fprintf(2,"[%d, %d] \t [%d, %d] \t [%d, %d]\n",pid1,10,pid2,20,pid3,30);
  fprintf(2,"-------------------------------------------\n");

  // Start in 1 to avoid inprecision because the first proccess started earlier
  count = 1;
  while(count < 2800000000LL){
    if(count % 200000000 == 0) printticks(pid1,pid2,pid3); 
    count++;
  }
  fprintf(2,"\n");
  wait(&pid1);
  wait(&pid2);
  wait(&pid3);
  exit(0);
}
