#pragma once

#include <pthread.h>
#include <cstdlib>
#include <stdio.h>

#define    NUM_THREADS     10000

class Mutex {
public:
    Mutex();
    ~Mutex();

    void Lock();
    void UnLock();

private:
    pthread_mutex_t mu_;
    // No copying
    Mutex(const Mutex&);
    void operator=(const Mutex&);
};