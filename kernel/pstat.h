#ifndef __PSTAT_H__
#define __PSTAT_H__

#include "param.h"

struct pstat{
  int inuse[NPROC];     // Proccess in use
  int tickets[NPROC];   // Number of tickets
  int pid[NPROC];       // PID of each process 
  int time[NPROC];      // Time each process has accumulated
  char name[NPROC][16];	// Holds an identificative string for each process
};

#endif // __PSTAT_H__
