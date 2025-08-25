//
// Created by root on 7/28/25.
//

#include "DataPartitionHeaderPage.h"

#include <cstring>

DataPartitionHeaderPage::DataPartitionHeaderPage(const std::string &file_name, uint16_t partition_number)  : Page(file_name, partition_number, 0) {

    total_pages = data[2] << 8 | data[3];

    memcpy(free_row_pages_bitmap, data + 4, PARTITION::BITMAP_SIZE);
}

DataPartitionHeaderPage::~DataPartitionHeaderPage() = default;

unsigned int DataPartitionHeaderPage::getTotalPages() {
    return total_pages;
}

uint8_t * DataPartitionHeaderPage::getFreeRowPagesBitmap() {
    return free_row_pages_bitmap;
}
