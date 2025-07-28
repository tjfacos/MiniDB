//
// Created by root on 7/28/25.
//

#include "Pointer.h"

Pointer::Pointer(unsigned int file, unsigned int partition, unsigned int page, unsigned int slot) : file(file), partition(partition), page(page), slot(slot) {}

unsigned int Pointer::getFile() const {
    return file;
}

unsigned int Pointer::getPart() const {
    return partition;
}

unsigned int Pointer::getPage() const {
    return page;
}

unsigned int Pointer::getSlot() const {
    return slot;
}
