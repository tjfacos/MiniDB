//
// Created by root on 7/28/25.
//

#ifndef SLOTTEDPAGE_H
#define SLOTTEDPAGE_H
#include "Page.h"


class SlottedPage : public Page {

    unsigned int slot_size{};
    unsigned int num_slots{};

    uint8_t* bitmap{};
    size_t   bitmap_size{};

    public:

        SlottedPage(const std::string &file, int partition_number, int page_number);
        ~SlottedPage();

        off_t getSlotOffset(int slot) const;
};



#endif //SLOTTEDPAGE_H
