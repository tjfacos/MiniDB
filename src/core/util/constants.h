//
// Created by root on 7/26/25.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Constants pertaining to individual pages within Data and Index files */
namespace PAGE {

    /* 8 KiB Page Size */
    constexpr unsigned int PAGE_SIZE = 8192;

    /* One byte for flags, 2 bytes of slot size, 1 byte of padding */
    constexpr unsigned int HEADER_BASE_SIZE = 4;

    constexpr uint8_t DATA_PAGE_FLAG                    = 1;
    constexpr uint8_t DATA_HEADER_FLAG                  = 2;
    constexpr uint8_t DATA_PARTITION_HEADER_FLAG        = 3;
    constexpr uint8_t INDEX_PAGE_FLAG                   = 4;
    constexpr uint8_t INDEX_HEADER_FLAG                 = 5;
    constexpr uint8_t INDEX_PARTITION_HEADER_FLAG       = 6;

}

namespace PARTITION {

    constexpr unsigned int PAGES_PER_PARTITION = 2048   ;
    constexpr unsigned int BITMAP_SIZE = 256            ;

}

/* Constants pertaining to Database/Column data types  */
namespace TYPE {

    constexpr uint8_t INT_FLAG    = 1;
    constexpr uint8_t FLOAT_FLAG  = 2;
    constexpr uint8_t STRING_FLAG = 3;
    constexpr uint8_t BINARY_FLAG = 4;
    constexpr uint8_t BOOL_FLAG   = 5;

    constexpr uint8_t INT_SIZE    = 4;
    constexpr uint8_t FLOAT_SIZE  = 4;
    constexpr uint8_t BOOL_SIZE   = 1;

    constexpr uint8_t UNIQUE_FLAG    = 1;
    constexpr uint8_t NULLABLE_FLAG  = 2;

}

namespace INDEX {

    constexpr size_t BITMAP_SIZE = 256;

}

namespace POINTER {

    constexpr size_t SIZE = 9;

}

#endif //CONSTANTS_H
