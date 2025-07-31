//
// Created by Thomas Facos on 26/07/2025.
//

#ifndef PAGE_H
#define PAGE_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "core/util/constants.h"


/**
 * @brief Represents a page on disk
 */
class Page {

public:

    enum PageType {
        IndexPage,
        DataPage,
        IndexHeader,
        DataHeader,
        IndexPartitionHeader,
        DataPartitionHeader
    };

protected:

    std::unordered_map<uint8_t, PageType> pageTypeFromFlag{
            {PAGE::INDEX_PAGE_FLAG                  , IndexPage                 },
            {PAGE::DATA_PAGE_FLAG                   , DataPage                  },
            {PAGE::INDEX_HEADER_FLAG                , IndexHeader               },
            {PAGE::DATA_HEADER_FLAG                 , DataHeader                },
            {PAGE::INDEX_PARTITION_HEADER_FLAG      , IndexPartitionHeader      },
            {PAGE::DATA_PARTITION_HEADER_FLAG       , DataPartitionHeader       }
    };

    PageType type;

    std::string filePath;
    int partition_no    ;
    int page_no         ;

    uint8_t* data;
    std::mutex mutex;

public:

    Page(std::string filePath, int partition_n, int page_n);
    ~Page();

    int         getNumber() const;
    uint8_t*    getData()   const;
    std::mutex* getMutex();

};



#endif //PAGE_H
