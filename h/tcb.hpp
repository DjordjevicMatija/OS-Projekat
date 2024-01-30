//
// Created by os on 1/22/24.
//

//Deo koda preuzet sa casova vezbi

#ifndef PROJEKAT_TCB_HPP
#define PROJEKAT_TCB_HPP

#include "../lib/hw.h"
#include "memoryAllocator.hpp"
#include "scheduler.hpp"

typedef TCB* thread_t;

class TCB{
public:

    //destruktor - dealokacija steka
    ~TCB(){
        MemoryAllocator::getMemAlloc().mem_free(stack);
    }

    using Body = void (*)(void*);

    bool isFinished() const{
        return finished;
    }

    void setFinished(bool val){
        finished = val;
    }

    bool isBlocked() const{
        return blocked;
    }

    void setBlocked(bool val){
        blocked = val;
    }

    //kreira nit nad f-jom body i cuva pokazivan na nit u handle
    static TCB* createThread(TCB** handle, Body body, void* args, void* stack_space);

    //dobrovoljno predavanje procesora drugoj niti
    static void yield();

    //nit koja se trenutno izvrsava na procesoru
    static TCB* running;

    //lista zavrsenih (mrtvih) niti
    static ListElem* dead_threads_head;

    //dealociranje zavrsenih niti
    static void dealloc_dead_threads();

    //dodavanje zavrsenih niti u listu
    static void put_dead_thread();

private:

    //kontekst niti
    struct Context{
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* args;
    void *stack;
    Context context;
    bool finished;
    bool blocked;
    uint64 timeSlice;

    //kreiranje TCB-a
    static TCB* createTCB(Body body, void* args, void* stack_space);

    //promena konteksta
    static void contextSwitch(Context *oldContext, Context *runningContext);

    //predavanje procesora drugoj niti
    static void dispatch();

    friend class Riscv;
    friend class KernelSemaphore;

    //funkcija koja ce da pozove funkciju body
    static void threadWrapper();

    //lista niti koje cekaju
    ListElem* join_threads_head;

    //dodavanje niti koje cekaju
    void add_join_thread();

    //vracanje niti koje su cekale u red spremnih
    void return_join_threads();
};

#endif //PROJEKAT_TCB_HPP
