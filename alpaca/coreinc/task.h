#ifndef TASK_H
#define TASK_H

#include <codes.h>

enum tasks {
    GetFile = 1,
    PutFile = 2
};

ALPACA_STATUS process_task(char* buf);

#endif