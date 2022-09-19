#ifndef __PSTAT_H__
#define __PSTAT_H__

#include "param.h"

struct pstat{
  int tickets[NPROC];   // Number of tickets
  int pid[NPROC];       // PID of each process (if it is -1 this entry is not in use)
  int time[NPROC];      // Time each process has accumulated
};

#endif // __PSTAT_H__
