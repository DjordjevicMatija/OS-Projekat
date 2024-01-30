//
// Created by os on 9/12/23.
//

//Deo koda preuzet sa casova vezbi

#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/memoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/semaphore.hpp"

void Riscv::popSppSpie(bool kernel) {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap() {

    //izvlacenje argumenata
    uint64 volatile arg1, arg2, arg3, arg4;
    __asm__ volatile("mv %[arg4], a4" : [arg4] "=r" (arg4));
    __asm__ volatile("mv %[arg3], a3" : [arg3] "=r" (arg3));
    __asm__ volatile("mv %[arg2], a2" : [arg2] "=r" (arg2));
    __asm__ volatile("mv %[arg1], a1" : [arg1] "=r" (arg1));

    //grananje na razlicite sistemske pozive
    uint64 op_num;
    __asm__ volatile("mv %[op_num], a0" : [op_num] "=r" (op_num));

    //citamo scause
    uint64 volatile scause = r_scause();

    if(scause == 0x0000000000000009UL || scause == 0x0000000000000008UL) {
        //interrupt: NO, cause code: environment call from S-mode (9) or U-mode (8)

        //cuvamo narednu vrednost sepc i sstatus
        uint64 volatile sepc = r_sepc() + 4; //skok na instrukciju nakon ecall-a (zbog toga dodajemo 4)
        uint64 volatile sstatus = r_sstatus();

        //mem alloc
        if(op_num == 0x01){

            uint64 size = arg1;

            void* addr = MemoryAllocator::getMemAlloc().mem_alloc(size);

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (addr));
        }

        //mem free
        else if(op_num == 0x02){

            void* addr = (void*) arg1;

            int val = MemoryAllocator::getMemAlloc().mem_free(addr);

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (val));
        }

        //thread create
        else if(op_num == 0x11){

            thread_t* handle = (thread_t*) arg1;
            void(*start_routine)(void*) = (void(*)(void*)) arg2;
            void* arg = (void*) arg3;
            void* stack_space = (void*) arg4;

            TCB::createThread(handle, start_routine, arg, stack_space);

            int val = 0;
            if(!handle){
                val = -1;
            }

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (val));
        }

        //thread exit
        else if(op_num == 0x12){

            TCB::running->setFinished(true);
            if(TCB::running->join_threads_head) {
                TCB::running->return_join_threads();
            }
            TCB::put_dead_thread();
            TCB::dispatch();
        }

        //thread dispatch
        else if(op_num == 0x13){

            if(TCB::dead_threads_head) {
                TCB::dealloc_dead_threads();
            }
            TCB::dispatch();
        }

        //thread join
        else if(op_num == 0x14){

            thread_t tcb = (thread_t) arg1;

            tcb->add_join_thread();
        }

        //sem open
        else if(op_num == 0x21){

            sem_t* handle = (sem_t*) arg1;
            unsigned init = (unsigned) arg2;

            KernelSemaphore::createSem(handle, init);

            int val = 0;
            if(!handle){
                val = -1;
            }

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (val));
        }

        //sem close
        else if(op_num == 0x22){

            sem_t handle = (sem_t) arg1;

            int val;
            if(handle){
                handle->sem_close();
                val = 0;
            }
            else{
                val = -1;
            }

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (val));
        }

        //sem wait
        else if(op_num == 0x23){

            sem_t handle = (sem_t) arg1;

            int val;
            if(handle){
                handle->sem_wait();
                val = 0;
            }
            else{
                val = -1;
            }

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (val));
        }

        //sem signal
        else if(op_num == 0x24){

            sem_t handle = (sem_t) arg1;

            int val;
            if(handle){
                handle->sem_signal();
                val = 0;
            }
            else{
                val = -1;
            }

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (val));
        }

        //getc
        else if(op_num == 0x41){

            char c = __getc();

            __asm__ volatile("mv a0, %[ret]" : : [ret] "r" (c));
        }

        //putc
        else if(op_num == 0x42){

            char c = (char) arg1;
            __putc(c);
        }

        //user mode
        else if(op_num == 0x51){
            mc_sip(SIP_SSIP);
            w_sstatus(sstatus);
            mc_sstatus(SSTATUS_SPP);
            w_sepc(sepc);
            return;
        }

        //vracamo sstatus i sepc
        w_sstatus(sstatus);
        w_sepc(sepc);
    }

    else if(scause == 0x8000000000000001UL) {
        //interrupt: yes, cause code: supervisor software interrupt (timer)
//        TCB::timeSliceCounter++;
//        if(TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
//            uint64 sepc = r_sepc();
//            uint64 sstatus = r_sstatus();
//            TCB::timeSliceCounter = 0;
//            TCB::dispatch();
//            w_sstatus(sstatus);
//            w_sepc(sepc);
//        }
        mc_sip(SIP_SSIP);
    }

    else if (scause == 0x8000000000000009UL) {
        //interrupt: yes, cause code: supervisor external interrupt (console)
        console_handler();
    }

    else {
        //unexpected trap cause
    }

}



