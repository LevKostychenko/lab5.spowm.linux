#ifndef _LAB5_H_
#define _LAB5_H_
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <aio.h>
#include <signal.h>
#include <fcntl.h>
#include <string>

class AsyncIO 
{
public:
    virtual void AsyncIOManager();
    static void* FileReader(void* params);
    static void* FileWriter(void* params);

    int result_file_desc = open("./ResultFile.txt", O_WRONLY | O_APPEND, S_IRWXU);  
    std::string files_to_read[3] = {"./File1.txt", "./File2.txt", "./File3.txt"};
    const int files_count = 3;
    char buff_to_read[10000];
    struct sigaction read_compleate_signal;
    struct sigaction write_compleate_signal;
    int readding_file_index = -1;
};
#endif