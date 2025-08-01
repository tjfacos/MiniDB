#include "Page.h"

#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "common/util/logging.h"
#include "core/util/constants.h"


Page::Page(std::string filePath, uint16_t partition_n, uint16_t page_n) : filePath(std::move(filePath)), partition_no(partition_n), page_no(page_n) {

    // Open file
    int file = open(filePath.c_str(), O_RDWR);
    if (file == -1) util::error("Failed to load page; could not open " + filePath);

    // Calculate offset
    off_t offset = getPageOffset();

    // Map page data to buffer
    data = static_cast<uint8_t *>(mmap(nullptr, PAGE::PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, offset));
    if (data == MAP_FAILED) {
        util::error("Failed to load page; could not map " + filePath + " at offset " + std::to_string(offset));
    }

    // File descriptor is no longer needed
    close(file);

    // Set Page Type
    type = pageTypeFromFlag[data[0]];
}

Page::~Page() {
    // Unmap memory
    if (munmap(data, PAGE::PAGE_SIZE) == -1) {
        util::error("Failed to unmap " + filePath);
    }
}

std::string Page::getFilePath() const {
    return filePath;
}

uint16_t Page::getPartition() const {
    return partition_no;
}

Page::PageType Page::getType() const {
    return type;
}

off_t Page::getPageOffset() const {
    return (1 + partition_no * PARTITION::PAGES_PER_PARTITION + page_no) * PAGE::PAGE_SIZE;
}

uint16_t Page::getNumber() const {
    return page_no;
}

uint8_t *Page::getData() const {
    return data;
}

std::mutex *Page::getMutex() {
    return &mutex;
}
