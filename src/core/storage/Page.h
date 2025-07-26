//
// Created by Thomas Facos on 26/07/2025.
//

#ifndef PAGE_H
#define PAGE_H
#include <mutex>
#include <string>

#include "core/util/constants.h"


/**
 * @brief Represents a page on disk
 */
class Page {

    std::string filePath;
    int page_no;
    uint8_t* data;
    std::mutex mutex;

    unsigned int type{};
    unsigned int slot_size{};
    unsigned int num_slots{};

    uint8_t* bitmap{};

    void load_header();

    public:

    Page(std::string filePath, int page_n);
    ~Page();

    int         getNumber() const;
    uint8_t*    getData()   const;
    std::mutex* getMutex();

    off_t       getSlotOffset() const;

};



#endif //PAGE_H
