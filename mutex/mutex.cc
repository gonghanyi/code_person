#include "mutex.h"

static void PthreadCall(const char* label, int result) {
    if (result != 0) {
        fprintf(stderr, "pthread %s: %d\n", label, result);
    }
}

Mutex::Mutex() { PthreadCall("init mutex", pthread_mutex_init(&mu_, NULL)); }
Mutex::~Mutex() { PthreadCall("destroy mutex", pthread_mutex_destroy(&mu_)); }
void Mutex::Lock() { PthreadCall("lock", pthread_mutex_lock(&mu_)); }
void Mutex::UnLock() { PthreadCall("unlock", pthread_mutex_unlock(&mu_)); }
