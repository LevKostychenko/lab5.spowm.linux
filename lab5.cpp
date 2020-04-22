#include <dlfcn.h>
#include <iostream>
#include "lab5.h"

int main(int argc, char* argv[])
{
        void* handle = dlopen("./asyncio.so", RTLD_LAZY);

        if (handle != NULL)
        {
            AsyncIO* (*create)();
            void (*destroy)(AsyncIO*);

            create = (AsyncIO* (*)())dlsym(handle, "CreateObject");
            destroy = (void (*)(AsyncIO*))dlsym(handle, "DestroyObject");

            AsyncIO* asyncIO = (AsyncIO*)create();
            asyncIO->AsyncIOManager();
            destroy(asyncIO);

            dlclose(handle);
        }
        else 
        {
            std::cout << "Error" << std::endl;
        }

        return 1;
}