//
// Created by root on 7/28/25.
//

#include "Pointer.h"

#include <cstring>
#include <vector>

Pointer::Pointer(Page::PageType type, uint16_t file, uint16_t partition, uint16_t page, uint16_t slot) : type(type), file(file), partition(partition), page(page), slot(slot) {}

Page::PageType Pointer::getType() const {
    return type;
}

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

Pointer *Pointer::fromBuffer(void *buffer) {

    uint8_t page_type_flag;
    uint16_t file, partition, page, slot;

    const auto* ptr = static_cast<uint8_t *>(buffer);

    memcpy(&page_type_flag  , ptr, sizeof(page_type_flag))  ;   ptr += sizeof(page_type_flag);
    memcpy(&file            , ptr, sizeof(file          ))  ;   ptr += sizeof(file          );
    memcpy(&partition       , ptr, sizeof(partition     ))  ;   ptr += sizeof(partition     );
    memcpy(&page            , ptr, sizeof(page          ))  ;   ptr += sizeof(page          );
    memcpy(&slot            , ptr, sizeof(slot          ))  ;

    return new Pointer{
        Page::pageTypeFromFlag[page_type_flag],
        file,
        partition,
        page,
        slot
    };

}

void Pointer::toBuffer(void *buffer) {

    std::vector<uint8_t> src_buff{POINTER::SIZE};

    src_buff[0] = Page::pageFlagFromType[type];
    memcpy(src_buff.data() + 1  , &file     , sizeof(file       ));
    memcpy(src_buff.data() + 3  , &partition, sizeof(partition  ));
    memcpy(src_buff.data() + 5  , &page     , sizeof(page       ));
    memcpy(src_buff.data() + 7  , &slot     , sizeof(slot       ));

    memcpy(buffer, src_buff.data(), src_buff.size());

}
