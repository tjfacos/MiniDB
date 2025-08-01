//
// Created by root on 7/28/25.
//

#ifndef INDEXHEADERPAGE_H
#define INDEXHEADERPAGE_H

#include "Page.h"
#include "core/storage/pointer/Pointer.h"

class IndexHeaderPage : public Page {

    unsigned int total_partitions;

    uint8_t free_idx_pages_bitmap[INDEX::BITMAP_SIZE]; // Partitions with space for B+ Tree nodes
    uint8_t free_ovf_pages_bitmap[INDEX::BITMAP_SIZE]; // Partitions with space for overflow nodes

    uint16_t btree_degree;
    uint16_t key_size;

    Pointer* root_pointer;

public:

    explicit IndexHeaderPage(const std::string& file_name);

    unsigned int getTotalPartitions() const;

    uint8_t* getFreeIndexPagesBitmap();
    uint8_t* getFreeOverflowPagesBitmap();

    uint16_t getBTreeDegree() const;
    uint16_t getKeySize() const;

    Pointer *getRootPointer() const;


};



#endif //INDEXHEADERPAGE_H
