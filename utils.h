#ifndef UTILS_H
#define UTILS_


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT 2000          // Server listening port
#define NUM_EXTERNALS 4     // Number of external client processes
#define EPS 1e-3            // Convergence tolerance


struct msg{
    float T;     // Temperature 
    int done;    // Index indifying the process 
}; 

struct msg prepare_message(int i_Index, float i_Temperature); 

#endif
