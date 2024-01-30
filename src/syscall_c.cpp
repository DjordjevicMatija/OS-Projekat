//
// Created by os on 1/22/24.
//

#include "../h/syscall_c.hpp"

void* mem_alloc(size_t size){

    uint64 op_num = 0x01;
    size_t aligned_size = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    __asm__ volatile("mv a1, %[size]" : : [size] "r" (aligned_size));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (void*) ret;
}

int mem_free(void* adr){

    uint64 op_num = 0x02;
//    void* adr_arg = adr;

    __asm__ volatile("mv a1, %[adr]" : : [adr] "r" (adr));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;

}

int thread_create(
        thread_t* handle,
        void(*start_routine) (void*),
        void* arg
        ){

    uint64 op_num = 0x11;
//    thread_t* handle_arg = handle;
//    void(*start_routine_arg)(void*) = start_routine;
//    void* arg_arg = arg;
    void* stack_space = nullptr;
    if(start_routine){
        stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    }

    __asm__ volatile("mv a4, %[stack_space]" : : [stack_space] "r" (stack_space));
    __asm__ volatile("mv a3, %[arg]" : : [arg] "r" (arg));
    __asm__ volatile("mv a2, %[start_routine]" : : [start_routine] "r" (start_routine));
    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;

}

int thread_exit(){

    uint64 op_num = 0x12;

    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");
    return 0;
};

void thread_dispatch(){

    uint64 op_num = 0x13;

    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");
}

void thread_join(thread_t handle){
    uint64 op_num = 0x14;

    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");
}

int sem_open(sem_t* handle, unsigned init){
    uint64 op_num = 0x21;

    __asm__ volatile("mv a2, %[val]" : : [val] "r" (init));
    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int sem_close(sem_t handle){
    uint64 op_num = 0x22;

    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int sem_wait(sem_t id){
    uint64 op_num = 0x23;

    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (id));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int sem_signal(sem_t id){
    uint64 op_num = 0x24;

    __asm__ volatile("mv a1, %[handle]" : : [handle] "r" (id));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int time_sleep(time_t time){return 0;}

char getc(){
    uint64 op_num = 0x41;

    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");

    char ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

void putc(char c){
    uint64 op_num = 0x42;

    __asm__ volatile("mv a1, %[ch]" : : [ch] "r" (c));
    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");
}

void change_to_user_mode(){
    uint64 op_num = 0x51;

    __asm__ volatile("mv a0, %[op]" : : [op]"r" (op_num));
    __asm__ volatile("ecall");
}