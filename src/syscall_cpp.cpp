//
// Created by os on 1/28/24.
//

#include "../h/syscall_cpp.hpp"

void* operator new(size_t size){
    return mem_alloc(size);
}

void* operator new[](size_t size){
    return mem_alloc(size);
}

void operator delete(void* adr){
    mem_free(adr);
}

void operator delete[](void* adr){
    mem_free(adr);
}

Thread::Thread(void (*body)(void *), void *arg){
    this->body = body;
    this->arg = arg;
}

Thread::~Thread() noexcept {}

int Thread::start(){
    return thread_create(&myHandle, &startWrapper, this);
}

void Thread::join(){
    thread_join(myHandle);
}

void Thread::dispatch(){
    thread_dispatch();
}

Thread::Thread(){
    this->body = nullptr;
    this->arg = nullptr;
}

int Thread::sleep(time_t) {return 0;}

void Thread::startWrapper(void *arg) {
    Thread* thread = (Thread*) arg;
    if(thread->body){
        thread->body(thread->arg);
    }
    else{
        thread->run();
    }
}

Semaphore::Semaphore(unsigned int init){
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore(){
    sem_close(myHandle);
}

int Semaphore::wait(){
    return sem_wait(myHandle);
}

int Semaphore::signal(){
    return sem_signal(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c){
    ::putc(c);
}