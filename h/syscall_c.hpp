//
// Created by os on 1/22/24.
//

#ifndef PROJEKAT_SYSCALL_C_HPP
#define PROJEKAT_SYSCALL_C_HPP

#include "../lib/hw.h"
#include "tcb.hpp"
#include "semaphore.hpp"

void* mem_alloc(size_t size);

int mem_free(void* adr);

int thread_create(
        thread_t* handle,
        void(*start_routine) (void*),
        void* arg
        );

int thread_exit();

void thread_dispatch();

void thread_join(thread_t handle);

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

int time_sleep(time_t time);

const int EOF = -1;

char getc();

void putc(char c);

void change_to_user_mode();

#endif //PROJEKAT_SYSCALL_C_HPP
