//
// Created by os on 1/28/24.
//

#include "../h/semaphore.hpp"
#include "../h/riscv.hpp"

KernelSemaphore* KernelSemaphore::createSem(KernelSemaphore** handle, unsigned value){
    size_t sem_size = (sizeof(KernelSemaphore) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    KernelSemaphore* sem = (KernelSemaphore*) MemoryAllocator::getMemAlloc().mem_alloc(sem_size);

    sem->val = value;
    sem->blocked_threads_head = nullptr;
    sem->blocked_threads_tail = nullptr;

    *handle = sem;
    return sem;
}

void KernelSemaphore::sem_close(){
    for(ListElem* cur = blocked_threads_head; cur != nullptr;){
        Scheduler::put(cur->tcb);
        ListElem* old = cur;
        cur = cur->next;
        MemoryAllocator::getMemAlloc().mem_free(old);
    }

    blocked_threads_head = nullptr;
    blocked_threads_tail = nullptr;
}

void KernelSemaphore::sem_wait(){
    this->val--;
    if(val < 0){
        //ubaci nit u listu blokiranih
        size_t size = (sizeof(ListElem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
        ListElem* newTcb = (ListElem*) MemoryAllocator::getMemAlloc().mem_alloc(size);

        TCB::running->setBlocked(true);
        newTcb->tcb = TCB::running;
        newTcb->next = nullptr;

        if(!blocked_threads_head){
            blocked_threads_head = newTcb;
        }
        else{
            blocked_threads_tail->next = newTcb;
        }
        blocked_threads_tail = newTcb;

        //daj procesor nekoj drugoj niti
        TCB::dispatch();
    }
}

void KernelSemaphore::sem_signal(){
    this->val++;
    if(val <= 0){
        //vrati jednu nit iz liste blokiranih u spremne
        ListElem* retElem = blocked_threads_head;
        blocked_threads_head = blocked_threads_head->next;
        if(!blocked_threads_head){
            blocked_threads_tail = nullptr;
        }

        retElem->tcb->setBlocked(false);
        Scheduler::put(retElem->tcb);
        MemoryAllocator::getMemAlloc().mem_free(retElem);
    }
}