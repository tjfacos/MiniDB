//
// Created by root on 7/28/25.
//

#ifndef POINTER_H
#define POINTER_H
#include <cstdint>

#include "core/storage/page/Page.h"

class Pointer {

    Page::PageType  type        ;
    uint16_t        file        ;
    uint16_t        partition   ;
    uint16_t        page        ;
    uint16_t        slot        ;

public:

    Pointer(Page::PageType type, uint16_t file, uint16_t partition, uint16_t page, uint16_t slot);

    [[nodiscard]] Page::PageType    getType() const;
    [[nodiscard]] uint16_t          getFile() const;
    [[nodiscard]] uint16_t          getPart() const;
    [[nodiscard]] uint16_t          getPage() const;
    [[nodiscard]] uint16_t          getSlot() const;

    static Pointer *fromBuffer(void *buffer);
    void toBuffer(void* buffer);

};

#endif //POINTER_H
