//
// Created by Thomas Facos on 26/07/2025.
//

#ifndef PAGE_H
#define PAGE_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "core/util/constants.h"
#include "core/util/helpers.h"


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


    inline static std::unordered_map<uint8_t, PageType> pageTypeFromFlag{
            {PAGE::INDEX_PAGE_FLAG                  , IndexPage                 },
            {PAGE::DATA_PAGE_FLAG                   , DataPage                  },
            {PAGE::INDEX_HEADER_FLAG                , IndexHeader               },
            {PAGE::DATA_HEADER_FLAG                 , DataHeader                },
            {PAGE::INDEX_PARTITION_HEADER_FLAG      , IndexPartitionHeader      },
            {PAGE::DATA_PARTITION_HEADER_FLAG       , DataPartitionHeader       }
    };

    inline static std::unordered_map<PageType, uint8_t> pageFlagFromType = util::invertMap(pageTypeFromFlag);

protected:

    std::string filePath    ;
    uint16_t partition_no        ;
    uint16_t page_no             ;

    PageType type   ;
    uint8_t* data   ;
    std::mutex mutex;

public:

    Page(std::string filePath, uint16_t partition_n, uint16_t page_n);
    ~Page();

    std::string getFilePath()   const;
    uint16_t    getPartition()  const;
    uint16_t    getNumber()     const;

    PageType    getType()       const;
    uint8_t*    getData()       const;
    std::mutex* getMutex();

    off_t getPageOffset()       const;
};



#endif //PAGE_H
