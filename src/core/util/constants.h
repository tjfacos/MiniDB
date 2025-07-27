//
// Created by root on 7/26/25.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace PAGE {

    /* 8 KiB Page Size */
    #define PAGE_SIZE 8192

    /* One byte for flags, 2 bytes of slot size, 1 byte of padding */
    #define HEADER_BASE_SIZE 4


}

namespace TYPE {

    #define INT_FLAG    1
    #define FLOAT_FLAG  2
    #define STRING_FLAG 3
    #define BINARY_FLAG 4
    #define BOOL_FLAG   5

    #define INT_SIZE    4
    #define FLOAT_SIZE  4
    #define BOOL_SIZE   1

    #define UNIQUE_FLAG     1
    #define NULLABLE_FLAG   2

}

namespace SCHEMA {

}

#endif //CONSTANTS_H
