#include "mutex.h"

class MutexLock {
public:
    explicit MutexLock(Mutex* mu) : mu_(mu) {
        mu_->Lock();
    }
    ~MutexLock() {
        mu_->UnLock();
    }
private:
    Mutex *const mu_;
    MutexLock(const MutexLock&);
    void operator=(const MutexLock&);
};