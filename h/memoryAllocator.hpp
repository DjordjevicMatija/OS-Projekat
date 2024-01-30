//
// Created by os on 9/13/23.
//

#ifndef PROJEKAT_MEMORYALLOCATOR_HPP
#define PROJEKAT_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct FreeMemHeader{
    FreeMemHeader* next;
    FreeMemHeader* prev;
    size_t size;
};

class MemoryAllocator {
public:
    //dohvatanje singleton klase
    static MemoryAllocator& getMemAlloc();

    //alokacije memorije
    void* mem_alloc(size_t size);

    //dealokacija memorije
    int mem_free(void* addr);

private:
    //pokazivac na pocetak liste slobodnih fragmenata
    FreeMemHeader* freeMemHead = nullptr;

    //konstruktor
    explicit MemoryAllocator();

    //pomocna f-ja za spajanje slobodnih fragmenata
    void tryToJoin(FreeMemHeader* cur);
};


#endif //PROJEKAT_MEMORYALLOCATOR_HPP
