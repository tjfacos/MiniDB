//
// Created by root on 7/28/25.
//

#include "Pointer.h"

Pointer::Pointer(uint16_t file, uint16_t partition, uint16_t page, uint16_t slot) : file(file), partition(partition), page(page), slot(slot) {}

uint16_t Pointer::getFile() const {
    return file;
}

uint16_t Pointer::getPart() const {
    return partition;
}

uint16_t Pointer::getPage() const {
    return page;
}

uint16_t Pointer::getSlot() const {
    return slot;
}
