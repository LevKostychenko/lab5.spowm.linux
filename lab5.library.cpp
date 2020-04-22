#include "lab5.h"

AsyncIO* asyncIO = new AsyncIO();
void* FileReader(void* params);
void thread_handler(int);
void read_handler(union sigval sv);
void write_handler(union sigval sv);
void test_handler(int sig, siginfo_t *si, void *ucontext);
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
    printf("0\n");
    // memset(&asyncIO->read_compleate_signal, 0, sizeof(asyncIO->read_compleate_signal));    
    // asyncIO->read_compleate_signal.sa_flags = 0;    
    // asyncIO->read_compleate_signal.sa_handler = write_handler;    
    // sigemptyset(&asyncIO->read_compleate_signal.sa_mask);
    // asyncIO->read_compleate_signal.sa_sigaction = test_handler;

    memset(&asyncIO->write_compleate_signal, 0, sizeof(asyncIO->write_compleate_signal));    
    //asyncIO->write_compleate_signal.sa_sigaction = read_handler;
    asyncIO->write_compleate_signal.sa_flags = 0;
    asyncIO->write_compleate_signal.sa_handler = thread_handler; 
    asyncIO->write_compleate_signal.sa_sigaction = test_handler;   
    sigemptyset(&asyncIO->write_compleate_signal.sa_mask);

    if(sigaction(SIGUSR1, &asyncIO->read_compleate_signal, NULL) == -1)
    {
        printf("sig error\n");
    }

    if(sigaction(SIGUSR2, &asyncIO->write_compleate_signal, NULL) == -1)
    {
        printf("sig error\n");
    }

    pthread_t thread;
    int status = pthread_create(&thread, NULL, &AsyncIO::FileReader, asyncIO);            
    sleep(2);
}

void* AsyncIO::FileReader(void* params)
{   
    asyncIO->readding_file_index ++;
    printf("File: %s\n", asyncIO->files_to_read[asyncIO->readding_file_index].c_str());
    int file = open(asyncIO->files_to_read[asyncIO->readding_file_index].c_str(), O_RDWR);
    struct aiocb _aiocb;
    memset(asyncIO->buff_to_read, 0xaa, 10000);
    _aiocb.aio_fildes = file;
    _aiocb.aio_buf = asyncIO->buff_to_read;
    _aiocb.aio_nbytes = 10;
    _aiocb.aio_reqprio = 0;
    //_aiocb.aio_lio_opcode = LIO_WRITE;
    _aiocb.aio_offset = 0;
    _aiocb.aio_sigevent.sigev_notify = SIGEV_THREAD;
    _aiocb.aio_sigevent.sigev_signo = SIGUSR1;
    _aiocb.aio_sigevent.sigev_value.sival_ptr = &_aiocb;//&asyncIO->read_compleate_signal;
    _aiocb.aio_sigevent.sigev_notify_function = &write_handler;
    if(aio_read(&_aiocb) == -1)
    {
        printf("aio_read error\n");
    }        
}

void write_handler(union sigval sv)
{
    printf("write_handle\n");
    if (asyncIO->readding_file_index <= asyncIO->files_count)
    {   
        struct aiocb _aiocb;
        _aiocb.aio_fildes = asyncIO->result_file_desc;
        _aiocb.aio_buf = asyncIO->buff_to_read;
        _aiocb.aio_nbytes = GetBufferLength(asyncIO->buff_to_read);
        _aiocb.aio_reqprio = 0;
        _aiocb.aio_lio_opcode = LIO_WRITE;
        _aiocb.aio_offset = 1;
        _aiocb.aio_sigevent.sigev_notify = SIGEV_THREAD;
        _aiocb.aio_sigevent.sigev_signo = SIGUSR2;
        _aiocb.aio_sigevent.sigev_value.sival_ptr = &_aiocb;
        _aiocb.aio_sigevent.sigev_notify_function = &read_handler;

        printf("%s\n", asyncIO->buff_to_read);
        if(aio_write(&_aiocb) == -1)
        {
            printf("aio_write error\n");
        }
        sleep(1);
        printf("%d\n", close(asyncIO->result_file_desc));
    }        
}

void read_handler(union sigval sv)
{
    printf("read_handle\n");
    if (asyncIO->readding_file_index < asyncIO->files_count)
    {
        asyncIO->readding_file_index ++;
        int file = open(asyncIO->files_to_read[asyncIO->readding_file_index].c_str(), O_RDONLY);

        struct aiocb _aiocb;
        memset(asyncIO->buff_to_read, 0xaa, 10000);
        memset(&asyncIO->buff_to_read, 0, sizeof(struct aiocb));
        _aiocb.aio_fildes = file;
        _aiocb.aio_buf = asyncIO->buff_to_read;
        _aiocb.aio_nbytes = 10000;
        _aiocb.aio_lio_opcode = LIO_WRITE;
        _aiocb.aio_offset = (intptr_t)-1;
        _aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        _aiocb.aio_sigevent.sigev_signo = SIGUSR1;
        _aiocb.aio_sigevent.sigev_value.sival_ptr = &_aiocb;
        _aiocb.aio_sigevent.sigev_notify_function = &write_handler;

        if(aio_read(&_aiocb) == -1)
        {
            printf("aio_read error\n");
        }
    }
}

int GetBufferLength(char char_array[])
{
    int counter = 0;

    for (int i = 0; char_array[i] > 64 && char_array[i] < 176; i++)
    {
        counter ++;
    }

    return counter;
}

void thread_handler(int a) 
{
        printf("%s\n", asyncIO->buff_to_read);
}


void test_handler(int sig, siginfo_t *si, void *ucontext)
{
    printf("Test!!!\n");
}