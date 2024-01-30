//
// Created by os on 1/28/24.
//

#ifndef PROJECT_BASE_SEMAPHORE_HPP
#define PROJECT_BASE_SEMAPHORE_HPP

#include "tcb.hpp"
#include "scheduler.hpp"

class KernelSemaphore{
public:

    //kreira semafor sa zadatom vrednoscu brojaca i cuva pokazivac u zadati handle
    static KernelSemaphore* createSem(KernelSemaphore** handle, unsigned value);

    //zatvara semafor
    void sem_close();

    //signal
    void sem_signal();

    //wait
    void sem_wait();

private:

    //interna vrednost brojaca
    int val;

    //pocetak liste blokiranih niti
    ListElem* blocked_threads_head;

    //kraj liste blokiranih niti
    ListElem* blocked_threads_tail;
};

typedef KernelSemaphore* sem_t;

#endif //PROJECT_BASE_SEMAPHORE_HPP
