#include "Page.h"

#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "common/util/logging.h"

void Page::load_header() {

    type        = data[0];                  // Get type
    slot_size   = data[1] << 8 | data[2];   // Get the size of each slot

    // Calculate Num Slots
    unsigned int N_p = (PAGE_SIZE - HEADER_BASE_SIZE) / slot_size;
    auto bitmap_size = static_cast<unsigned int>(std::ceil(static_cast<double>(N_p) / 8));
    num_slots = N_p - bitmap_size;

    // Get bitmap
    bitmap = new uint8_t[bitmap_size];
    memcpy(bitmap, data + HEADER_BASE_SIZE, bitmap_size);

}

Page::Page(std::string filePath, int page_n) : filePath(std::move(filePath)), page_no(page_n) {

    // Open file
    int file = open(filePath.c_str(), O_RDWR);
    if (file == -1) util::error("Failed to load page; could not open " + filePath);

    // Calculate offset
    off_t offset = page_n * PAGE_SIZE;

    // Map page data to buffer
    data = static_cast<uint8_t *>(mmap(nullptr, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, offset));
    if (data == MAP_FAILED) {
        util::error("Failed to load page; could not map " + filePath + " at offset " + std::to_string(offset));
    }

    // File descriptor is no longer needed
    close(file);

    load_header();
}

Page::~Page() {
    // Unmap memory
    if (munmap(data, PAGE_SIZE) == -1) {
        util::error("Failed to unmap " + filePath);
    }
}

int Page::getNumber() const {
    return page_no;
}

uint8_t *Page::getData() const {
    return data;
}

std::mutex *Page::getMutex() {
    return &mutex;
}
