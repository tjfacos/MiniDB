//
// Created by root on 7/28/25.
//

#include "SlottedPage.h"

#include <cmath>

SlottedPage::SlottedPage(const std::string& file, int partition_number, int page_number): Page(file, partition_number, page_number) {

    slot_size   = data[1] << 8 | data[2]        ;   // Get the size of each slot
    bitmap      = data + PAGE::HEADER_BASE_SIZE ;   // Set bitmap pointer

    const unsigned int N_p = (PAGE::PAGE_SIZE - PAGE::HEADER_BASE_SIZE) / slot_size;

    // Calculate bitmap size
    bitmap_size = static_cast<unsigned int>(std::ceil(static_cast<double>(N_p) / 8));

    // Calculate Num Slots
    num_slots = (PAGE::PAGE_SIZE - PAGE::HEADER_BASE_SIZE - bitmap_size) / slot_size;

}

SlottedPage::~SlottedPage() = default;

off_t SlottedPage::getSlotOffset(int slot) const {
    return (partition_no * PARTITION::PAGES_PER_PARTITION +  page_no) * PAGE::PAGE_SIZE + slot * slot_size;
}
