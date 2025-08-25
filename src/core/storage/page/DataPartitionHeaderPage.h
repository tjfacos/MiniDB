
#ifndef DATAPARTITIONHEADERPAGE_H
#define DATAPARTITIONHEADERPAGE_H

#include "Page.h"

class DataPartitionHeaderPage : public Page {

    unsigned int total_pages;

    uint8_t free_row_pages_bitmap[PARTITION::BITMAP_SIZE]{}; // Pages with space for new row nodes

public:

    DataPartitionHeaderPage(const std::string &file_name, uint16_t partition_number);
    ~DataPartitionHeaderPage();

    unsigned int getTotalPages();
    uint8_t* getFreeRowPagesBitmap();


};



#endif //DATAPARTITIONHEADERPAGE_H
