//
// Created by root on 7/28/25.
//

#include "IndexPartitionHeaderPage.h"

#include <cstring>

IndexPartitionHeaderPage::IndexPartitionHeaderPage(const std::string &file_name, uint16_t partition_number) : Page(file_name, partition_number, 0) {

    total_pages = data[2] << 8 | data[3];

    memcpy(free_idx_pages_bitmap, data + 4, PARTITION::BITMAP_SIZE);
    memcpy(free_ovf_pages_bitmap, data + 4 + PARTITION::BITMAP_SIZE, PARTITION::BITMAP_SIZE);

}

IndexPartitionHeaderPage::~IndexPartitionHeaderPage() = default;

unsigned int IndexPartitionHeaderPage::getTotalPages() {
    return total_pages;
}

uint8_t * IndexPartitionHeaderPage::getFreeIndexPagesBitmap() {
    return free_idx_pages_bitmap;
}

uint8_t * IndexPartitionHeaderPage::getFreeOverflowPagesBitmap() {
    return free_ovf_pages_bitmap;
}
