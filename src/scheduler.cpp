//Deo koda preuzet sa casova vezbi

#include "../h/scheduler.hpp"
#include "../h/memoryAllocator.hpp"

ListElem* Scheduler::head = nullptr;
ListElem* Scheduler::tail = nullptr;

TCB *Scheduler::get() {
    if(!head){
        return nullptr;
    }

    ListElem* retElem = head;
    head = head->next;
    if(!head){
        tail = nullptr;
    }

    TCB* ret = retElem->tcb;
    MemoryAllocator::getMemAlloc().mem_free(retElem);
    return ret;
}

void Scheduler::put(TCB *tcb) {
    size_t size = (sizeof(ListElem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    ListElem* newTcb = (ListElem*) MemoryAllocator::getMemAlloc().mem_alloc(size);
    newTcb->tcb = tcb;
    newTcb->next = nullptr;
    if(!head){
        head = newTcb;
    }
    else{
        tail->next = newTcb;
    }
    tail = newTcb;
}
