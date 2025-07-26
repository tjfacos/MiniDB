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

    public:

    Page(std::string filePath, int page_n);
    ~Page();

    int         getNumber() const;
    uint8_t*    getData() const;
    std::mutex* getMutex();

};



#endif //PAGE_H
