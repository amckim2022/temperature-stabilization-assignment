#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include "utils.h"


// Simple helper function for building a message struct
struct msg prepare_message(int i_Index, float i_Temperature) {
    struct msg message;
    message.T = i_Temperature;
    message.done = 0;   // always 0 when preparing a message
    return message;
}

