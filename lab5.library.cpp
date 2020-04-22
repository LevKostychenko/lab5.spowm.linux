#include "lab5.h"

AsyncIO* asyncIO = new AsyncIO();
void* FileReader(void* params);
void* read_handler(void*);
void* write_handler(void*);
int GetBufferLength(char char_array[]);

extern "C" 
{
    AsyncIO* CreateObject()
    {
        return new AsyncIO;
    }

    void DestroyObject(AsyncIO* object)
    {
        delete object;
    }
}

void AsyncIO::AsyncIOManager()
{
    pthread_t thread;
    int status = pthread_create(&thread, NULL, &read_handler, asyncIO);            
    sleep(2);
    printf("%d\n", close(asyncIO->result_file_desc));
}

void* write_handler(void* param)
{
    printf("write_handle\n");
    if (asyncIO->readding_file_index <= asyncIO->files_count)
    {   
        struct aiocb _aiocb;
        _aiocb.aio_fildes = asyncIO->result_file_desc;
        _aiocb.aio_buf = asyncIO->buff_to_read;
        _aiocb.aio_nbytes =10;
        _aiocb.aio_reqprio = 0;
        _aiocb.aio_lio_opcode = LIO_WRITE;
        _aiocb.aio_offset = 1;

        printf("%s\n", asyncIO->buff_to_read);
        if(aio_write(&_aiocb) == -1)
        {
            printf("aio_write error\n");
        }
        while (aio_error(&_aiocb) == EINPROGRESS) { printf ("wait\n");}
        read_handler(NULL);        
    }        
}

void* read_handler(void* param)
{
    asyncIO->readding_file_index ++;
    printf("read_handle\n");
    if (asyncIO->readding_file_index < asyncIO->files_count)
    {
       int file = open(asyncIO->files_to_read[asyncIO->readding_file_index].c_str(), O_RDWR);
        struct aiocb _aiocb;
        memset(asyncIO->buff_to_read, 0xaa, 10000);
        _aiocb.aio_fildes = file;
        _aiocb.aio_buf = asyncIO->buff_to_read;
        _aiocb.aio_nbytes = 10;
        _aiocb.aio_reqprio = 0;
        _aiocb.aio_lio_opcode = LIO_READ;
        _aiocb.aio_offset = 0;
        
        if(aio_read(&_aiocb) == -1)
        {
            printf("aio_read error\n");
        }
        
        while (aio_error(&_aiocb) == EINPROGRESS) { printf ("wait\n");}

        write_handler(NULL);
    }
}

int GetBufferLength(char char_array[])
{
    int counter = 0;

    for (int i = 0; (char_array[i] > 64 && char_array[i] < 176) ||  char_array[i] == ' '; i++)
    {
        counter ++;
    }

    return counter;
}
