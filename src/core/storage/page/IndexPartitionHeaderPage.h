//
// Created by root on 7/28/25.
//

#ifndef INDEXPARTITIONHEADERPAGE_H
#define INDEXPARTITIONHEADERPAGE_H
#include <cstdint>

#include "Page.h"
#include "core/util/constants.h"


class IndexPartitionHeaderPage : public Page {

    unsigned int total_pages;

    uint8_t free_idx_pages_bitmap[PARTITION::BITMAP_SIZE]{}; // Pages with space for B+ Tree nodes
    uint8_t free_ovf_pages_bitmap[PARTITION::BITMAP_SIZE]{}; // Pages with space for overflow nodes

public:

    IndexPartitionHeaderPage(const std::string &file_name, uint16_t partition_number);
    ~IndexPartitionHeaderPage();

    unsigned int getTotalPages();
    uint8_t* getFreeIndexPagesBitmap();
    uint8_t* getFreeOverflowPagesBitmap();

};



#endif //INDEXPARTITIONHEADERPAGE_H
