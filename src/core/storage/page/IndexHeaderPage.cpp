//
// Created by root on 7/28/25.
//

#include "IndexHeaderPage.h"

#include <cstring>

IndexHeaderPage::IndexHeaderPage(const std::string& file_name) : Page(file_name, 0, 0) {

    // Load total partitions

    total_partitions = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];

    // Load bitmaps

    memcpy(free_idx_pages_bitmap, data + 8, INDEX::BITMAP_SIZE);
    memcpy(free_ovf_pages_bitmap, data + 8 + INDEX::BITMAP_SIZE, INDEX::BITMAP_SIZE);

    // Load B+ Tree degree and Key Size

    off_t post_bitmaps = 8  + INDEX::BITMAP_SIZE * 2;
    btree_degree    = data[post_bitmaps     ] << 8 | data[post_bitmaps + 1];
    key_size        = data[post_bitmaps + 2 ] << 8 | data[post_bitmaps + 3];

    // Load root Pointer

    root_pointer = Pointer::fromBuffer(data + post_bitmaps + 4);
}

unsigned int IndexHeaderPage::getTotalPartitions() const {
    return total_partitions;
}

uint8_t * IndexHeaderPage::getFreeIndexPagesBitmap() {
    return free_idx_pages_bitmap;
}

uint8_t * IndexHeaderPage::getFreeOverflowPagesBitmap() {
    return free_ovf_pages_bitmap;
}

uint16_t IndexHeaderPage::getBTreeDegree() const {
    return btree_degree;
}

uint16_t IndexHeaderPage::getKeySize() const {
    return key_size;
}

Pointer* IndexHeaderPage::getRootPointer() const {
    return root_pointer;
}
