#ifndef TASK_H
#define TASK_H

#include <nscodes.h>

enum tasks {
    GetFile = 1,
    PutFile = 2
};

NS_STATUS process_task(char* buf);

#endif