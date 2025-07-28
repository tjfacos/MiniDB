//
// Created by root on 7/28/25.
//

#include "IndexHeaderPage.h"

#include <cstring>

IndexHeaderPage::IndexHeaderPage(const std::string& file_name) : Page(file_name, 0, 0) {

    // Load total partitions

    total_partitions = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];

    // Load bitmaps

    memcpy(full_idx_pages_bitmap, data + 8, INDEX::BITMAP_SIZE);
    memcpy(full_ovf_pages_bitmap, data + 8 + INDEX::BITMAP_SIZE, INDEX::BITMAP_SIZE);

    // Load B+ Tree degree and Key Size

    off_t post_bitmaps = 8  + INDEX::BITMAP_SIZE * 2;
    btree_degree    = data[post_bitmaps     ] << 8 | data[post_bitmaps + 1];
    key_size        = data[post_bitmaps + 2 ] << 8 | data[post_bitmaps + 3];

    // Load root Pointer

    uint32_t file;
    uint32_t partition;
    uint32_t page;
    uint32_t offset;

    memcpy(&file        , &data[post_bitmaps + 4 ], sizeof(uint32_t));
    memcpy(&partition   , &data[post_bitmaps + 8 ], sizeof(uint32_t));
    memcpy(&page        , &data[post_bitmaps + 12], sizeof(uint32_t));
    memcpy(&offset      , &data[post_bitmaps + 16], sizeof(uint32_t));

    root_pointer = new Pointer(file, partition, page, offset);

}

unsigned int IndexHeaderPage::getTotalPartitions() const {
    return total_partitions;
}

uint8_t * IndexHeaderPage::getFullIndexPagesBitmap() {
    return full_idx_pages_bitmap;
}

uint8_t * IndexHeaderPage::getFullOverflowPagesBitmap() {
    return full_ovf_pages_bitmap;
}

uint16_t IndexHeaderPage::getBTreeDegree() const {
    return btree_degree;
}

uint16_t IndexHeaderPage::getKeySize() const {
    return key_size;
}

Pointer *IndexHeaderPage::getRootPointer() const {
    return root_pointer;
}
