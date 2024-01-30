//Deo koda uzet sa casova vezbi

#ifndef OS1_VEZBE07_RISCV_CONTERXT_SWITCH_1_SYNCHRONOUS_SCHEDULER_HPP
#define OS1_VEZBE07_RISCV_CONTERXT_SWITCH_1_SYNCHRONOUS_SCHEDULER_HPP

class TCB;

struct ListElem{
    ListElem* next;
    TCB* tcb;
};

class Scheduler {
private:

    //pokazivac na pocetak liste
    static ListElem* head;

    //pokazivac na kraj liste
    static ListElem* tail;

public:

    //dohvatanje niti iz reda spremnih
    static TCB* get();

    //stavljanje niti u red spremnih
    static void put(TCB *tcb);

};

#endif