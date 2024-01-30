//
// Created by os on 9/13/23.
//

#include "../h/memoryAllocator.hpp"

MemoryAllocator::MemoryAllocator() {
    freeMemHead = (FreeMemHeader*) ((uint64) HEAP_START_ADDR);
    freeMemHead->next = nullptr;
    freeMemHead->prev = nullptr;
    freeMemHead->size = (uint64) ((uint64) HEAP_END_ADDR - (uint64) HEAP_START_ADDR - sizeof(MemoryAllocator));
}

MemoryAllocator& MemoryAllocator::getMemAlloc() {
    static MemoryAllocator singleton;
    return singleton;
}

void* MemoryAllocator::mem_alloc(size_t size) {
	if(size <= 0) return nullptr;

    size_t bit_size = sizeof(FreeMemHeader) + size * MEM_BLOCK_SIZE;
	
    for(FreeMemHeader* cur = freeMemHead; cur != nullptr; cur = cur->next){
        if(cur->size >= bit_size){
            void* addr = cur;
            size_t remainingSize = cur->size - bit_size;
            // ako je ostalo slobodnog mesta za makar jedan blok
            if(remainingSize > sizeof(FreeMemHeader)){
                FreeMemHeader* newFreeMemHeader = (FreeMemHeader*) ((uint64) cur + bit_size);
                if(cur->prev){
                    cur->prev->next = newFreeMemHeader;
                }
                else{
                    freeMemHead = newFreeMemHeader;
                }
                if(cur->next){
                    cur->next->prev = newFreeMemHeader;
                }

                newFreeMemHeader->prev = cur->prev;
                newFreeMemHeader->next = cur->next;
                newFreeMemHeader->size = remainingSize;
            }
            // ako nije ostalo slobodnog mesta u bloku
            else{
                if(cur->prev){
                    cur->prev->next = cur->next;
                }
                else{
                    freeMemHead = cur->next;
                }
                if(cur->next){
                    cur->next->prev = cur->prev;
                }
            }

            cur->size = bit_size;
            return (char*) addr + sizeof(FreeMemHeader);
        }
    }
    return nullptr;
}

void MemoryAllocator::tryToJoin(FreeMemHeader *cur) {
    if(!cur) return;
    if(cur->next && (uint64) cur + cur->size == (uint64) (cur->next)){
        cur->size += cur->next->size;
        cur->next = cur->next->next;
        if(cur->next){
            cur->next->prev = cur;
        }
    }

}

int MemoryAllocator::mem_free(void *addr) {
    uint64 address = (uint64) addr;
    if(!address || address < (uint64) HEAP_START_ADDR || address > (uint64) HEAP_END_ADDR){
        return -1;
    }

    FreeMemHeader* cur;
    if(!freeMemHead || address < (uint64) freeMemHead){
        cur = nullptr;
    }
    else{
        for(cur = freeMemHead; cur->next && address > (uint64) (cur->next); cur = cur->next);
    }

    FreeMemHeader* newFreeMemHeader = (FreeMemHeader*) (address - sizeof(FreeMemHeader));
    newFreeMemHeader->prev = cur;
    if(cur){
        newFreeMemHeader->next = cur->next;
    }
    else{
        newFreeMemHeader->next = freeMemHead;
    }
    if(newFreeMemHeader->next){
        newFreeMemHeader->next->prev = newFreeMemHeader;
    }
    if(cur){
        cur->next = newFreeMemHeader;
    }
    else{
        freeMemHead = newFreeMemHeader;
    }

    tryToJoin(newFreeMemHeader);
    tryToJoin(cur);
    return 0;
}
