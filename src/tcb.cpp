//
// Created by os on 1/22/24.
//

//Deo koda preuzet sa casova vezbi

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

TCB* TCB::running = nullptr;

TCB* TCB::createThread(TCB** handle, Body body, void* args, void* stack_space){
    TCB* tcb = createTCB(body, args, stack_space);
    *handle = tcb;
    if(body){
        Scheduler::put(tcb);
    }
    return tcb;
}

TCB* TCB::createTCB(Body body, void* args, void* stack_space){
    size_t tcb_size = (sizeof(TCB) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    TCB* tcb = (TCB*) MemoryAllocator::getMemAlloc().mem_alloc(tcb_size);

    tcb->body = body;
    tcb->args = args;
    tcb->stack = stack_space;
    tcb->context = {
            (uint64) &threadWrapper,
            tcb->stack != nullptr ? (uint64)tcb->stack + DEFAULT_STACK_SIZE : 0
    };
    tcb->finished = false;
    tcb->blocked = false;
    tcb->timeSlice = DEFAULT_TIME_SLICE;
    tcb->join_threads_head = nullptr;

    return tcb;
}

void TCB::yield(){
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

void TCB::dispatch(){
    TCB* old = running;
    if(!old->isFinished() && !old->isBlocked()){
        Scheduler::put(old);
    }
    running = Scheduler::get();

    if(old != running) {
        contextSwitch(&old->context, &running->context);
    }
}

void TCB::threadWrapper(){
    bool kernel = false;
    if(!running->body){
        kernel = true;
    }

    Riscv::popSppSpie(kernel);
    running->body(running->args);
    thread_exit();
}

ListElem* TCB::dead_threads_head = nullptr;

void TCB::dealloc_dead_threads(){
    for(ListElem* cur = dead_threads_head; cur != nullptr;){
        MemoryAllocator::getMemAlloc().mem_free(cur->tcb->stack);
        MemoryAllocator::getMemAlloc().mem_free(cur->tcb);
        ListElem* old = cur;
        cur = cur->next;
        MemoryAllocator::getMemAlloc().mem_free(old);
    }
    dead_threads_head = nullptr;
}

void TCB::put_dead_thread(){
    size_t size = (sizeof(ListElem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    ListElem* newTcb = (ListElem*) MemoryAllocator::getMemAlloc().mem_alloc(size);
    newTcb->next = dead_threads_head;
    newTcb->tcb = running;
    dead_threads_head = newTcb;
}

void TCB::add_join_thread(){
    //ubaci nit u listu
    size_t size = (sizeof(ListElem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    ListElem* newTcb = (ListElem*) MemoryAllocator::getMemAlloc().mem_alloc(size);
    newTcb->next = join_threads_head;
    newTcb->tcb = running;
    join_threads_head = newTcb;

    //daj procesor drugoj niti
    TCB* old = running;
    running = Scheduler::get();

    if(old != running) {
        contextSwitch(&old->context, &running->context);
    }
}

void TCB::return_join_threads(){
    for(ListElem* cur = join_threads_head; cur != nullptr;){
        Scheduler::put(cur->tcb);
        ListElem* old = cur;
        cur = cur->next;
        MemoryAllocator::getMemAlloc().mem_free(old);
    }
    join_threads_head = nullptr;
}

