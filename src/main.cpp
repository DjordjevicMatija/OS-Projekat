#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

extern void userMain();

thread_t handle_main;

//void idle(){
//    while(true){
//        thread_dispatch();
//    }
//}
//
//thread_t idle_thread;

void userMainWrapper(void* arg){
    userMain();
}

thread_t user_thread;

int main(){
    Riscv::w_stvec((uint64) &Riscv::vectorTable | 1); //reakcija na prekide
    Riscv::ms_sie(Riscv::SIE_SEIE); //omoguci prekide od tajmera
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);  //omogucava spoljasnje prekide

    thread_create(&handle_main, nullptr, nullptr);

    TCB::running = handle_main;

//    thread_create(&idle_thread, reinterpret_cast<void (*)(void *)>(idle), nullptr);

    change_to_user_mode();

    thread_create(&user_thread, &userMainWrapper, nullptr);

    while(!user_thread->isFinished()){
        thread_dispatch();
    }

    return 0;
}
